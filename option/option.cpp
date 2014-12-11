///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "option.hpp"
#include "string.hpp"

#include <iomanip>
#include <sstream>

#include <getopt.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
int option::generate()
{
    static int code = name_max;
    if(++code)
        return code;
    else throw std::runtime_error("option::generate(): overflow");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void options::parse(int argc, char* argv[], int& index)
{
    std::vector<struct ::option> long_opt;
    std::vector<std::unique_ptr<char[]>> long_opt_name;

    std::string short_opt(":");

    ////////////////////
    for(const_reference option: _M_c)
    {

        if(!option.generated())
        {
            short_opt += option.name();

            if(option.need_arg() == uncertain)
                short_opt += "::";
            else if(option.need_arg())
                short_opt += ":";
        }

        if(option.long_name().size())
        {
            long_opt_name.push_back(clone(option.long_name()));

            long_opt.push_back({ long_opt_name.back().get(),
                option.need_arg() == uncertain ? optional_argument : option.need_arg() ? required_argument : no_argument,
            nullptr, option.code() });
        }
    }
    long_opt.push_back({ nullptr, 0, nullptr, 0 });

    ////////////////////
    try
    {
        optind = 1;
        opterr = 0;

        while(true)
        {
            int c = getopt_long(argc, argv, short_opt.data(), &long_opt[0], nullptr);
            if(c == -1) break;

            if(c == '?')
                throw std::invalid_argument("Invalid option");
            else if(c == ':')
                throw std::invalid_argument("Missing argument");

            for(reference option: _M_c)
            {
                if(c == option.code())
                {
                    if(option.single() && option.count())
                        throw std::invalid_argument("Duplicate option");
                    option.inc_count();

                    if(option.need_arg() != false && optarg)
                    {
                        option.assign(optarg);
                        option.set_have();
                    }
                    break;
                }
            }
        }
    }
    catch(std::exception& e)
    {
        index = optind;

        std::string message = e.what();
        message += " '";
            if(optopt && optopt <= option::name_max)
                message += char(optopt);
            else message += argv[--index];
        message += "'";

        throw std::invalid_argument(message);
    }

    index = optind;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
using std::setw; using std::left; using std::right; using std::endl;

std::string options::usage() const
{
    std::ostringstream stream;
    for(const_reference option: _M_c)
    {
        stream << setw(8) << right;
        if(option.name())
            stream << std::string("-") + option.name() + (option.long_name().size() ? ", " : "  ");
        else stream << ' ';

        stream << setw(20) << left;
        if(option.long_name().size())
            stream << std::string("--") + option.long_name() + (option.need_arg() == uncertain ? "[=arg]" : option.need_arg() ? "=<arg>" : "");
        else stream << ' ';

        stream << option.description() << std::endl;
    }

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
