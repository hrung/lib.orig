///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stream.h"
#include "option.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <map>

#include <getopt.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace opt
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void parse(int argc, char* argv[], options& opts, int& index)
{
    std::vector<::option> long_opt;
    std::string short_opt(":");

    int unique= 256;
    std::map<int, option*> tmp;

    ////////////////////
    for(option& opt: opts)
    {
        int val;
        if(opt.short_name)
        {
            val= opt.short_name;
            short_opt+= opt.short_name;

            switch(opt.arg)
            {
                case optional: short_opt+= ':';
                case required: short_opt+= ':';
                case no: ;
            }
        }
        else val= unique++;

        if(opt.name.size()) long_opt.push_back({ opt.name.data(), opt.arg, nullptr, val });
        tmp[val]= &opt;
    }
    long_opt.push_back({0, 0, 0, 0});

    ////////////////////
    try
    {
        optind=1;
        opterr=0;

        while(true)
        {
            int c= getopt_long(argc, argv, short_opt.data(), &long_opt[0], nullptr);
            if(c==-1) break;

            switch(c)
            {
            case '?': throw std::invalid_argument("Invalid option");
            case ':': throw std::invalid_argument("Missing argument");
            }

            auto ri= tmp.find(c);
                if(ri==tmp.end()) throw std::invalid_argument("Unexpected option");
            option* popt= ri->second;

            std::string value;
            if( (popt->arg==required || popt->arg==optional) && optarg ) value= optarg;

            popt->value.push_back(value);
        }
    }
    catch(std::exception& e)
    {
        index= optind;

        stream error;
        error << e.what() << " '";
            if(optopt) error << char(optopt); else error << argv[--index];
        error << "'";

        throw std::invalid_argument(error);
    }

    index= optind;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
option_map map(const opt::options& options)
{
    option_map _M_map;

    for(const option& opt: options)
        _M_map[ opt.name.size()? opt.name: (std::string()+ opt.short_name) ]= &opt;
    return _M_map;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
using std::setw; using std::left; using std::right; using std::endl;

std::string usage(const options& opts)
{
    std::ostringstream stream;
    for(const option& opt: opts)
    {
        stream << setw(8) << right;
        if(opt.short_name)
            stream << std::string("-")+ opt.short_name+ (opt.name.size()? ", ": "  ");
        else stream << ' ';

        stream << setw(20) << left;
        if(opt.name.size())
            stream << std::string("--")+ opt.name+ (opt.arg==opt::required? "=<arg>": opt.arg==opt::optional? "[=arg]": "");
        else stream << ' ';

        if(opt.desc.size()) stream << opt.desc;
        stream << std::endl;
    }

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
