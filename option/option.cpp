///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "option.h"
#include "stream.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <map>

#include <getopt.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace opt
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void options::parse(int argc, char* argv[], int& index)
{
    std::vector<::option> long_opt;
    std::string short_opt(":");

    int unique= 256;
    std::map<int, option*> tmp;

    ////////////////////
    for(auto ri= _M_map.begin(); ri!=_M_map.end(); ++ri)
    {
        option* opt= &ri->second;

        int val;
        if(opt->_M_abbr)
        {
            val= opt->_M_abbr;
            short_opt+= opt->_M_abbr;

            switch(opt->_M_arg)
            {
                case optional: short_opt+= ':';
                case required: short_opt+= ':';
                case no: ;
            }
        }
        else val= unique++;

        if(!opt->_M_name.empty()) long_opt.push_back({ opt->_M_name.data(), opt->_M_arg, nullptr, val });
        tmp[val]= opt;
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
            option* opt= ri->second;

            std::string value;
            if( (opt->_M_arg==required || opt->_M_arg==optional) && optarg ) value= optarg;

            opt->_M_values.push_back(value);
        }
    }
    catch(std::exception& e)
    {
        index= optind;

        std::string error;
        error << e.what() << " '";
            if(optopt) error << char(optopt); else error << argv[--index];
        error << "'";

        throw std::invalid_argument(error);
    }

    index= optind;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
using std::setw; using std::left; using std::right; using std::endl;

std::string options::usage()
{
    std::ostringstream stream;
    for(auto ri: _M_map)
    {
        option& opt= ri.second;

        stream << setw(8) << right;
        if(opt._M_abbr)
            stream << std::string("-")+ opt._M_abbr+ (opt._M_name.size()? ", ": "  ");
        else stream << ' ';

        stream << setw(20) << left;
        if(opt._M_name.size())
            stream << std::string("--")+ opt._M_name+ (opt._M_arg==opt::required? "=<arg>": opt._M_arg==opt::optional? "[=arg]": "");
        else stream << ' ';

        if(opt._M_desc.size()) stream << opt._M_desc;
        stream << std::endl;
    }

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
