///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "entry.h"
#include "except.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace storage
{

///////////////////////////////////////////////////////////////////////////////////////////////////
const filter_func filter_all= [] (const entry&) -> bool
    { return true; };

const compare_func compare_version= [] (const entry& e1, const entry& e2) -> int
    { return strverscmp(e1.name.data(), e2.name.data()); };

const compare_func compare_alpha= [] (const entry& e1, const entry& e2) -> int
    { return strcoll(e1.name.data(), e2.name.data()); };

///////////////////////////////////////////////////////////////////////////////////////////////////
entries entry::get(const std::string& path, const filter_func& filter, const compare_func& compare)
{
    static filter_func _M_filter;
    static compare_func _M_compare;

    dirent** names= nullptr;
    _M_filter= filter;
    _M_compare= compare;

    int n= scandir(path.data(), &names,
        [] (const dirent* e) -> int { return _M_filter({ e->d_name, static_cast<storage::type>(e->d_type), e->d_ino }); },
        [] (const dirent** e1, const dirent** e2) -> int
        {
            return _M_compare( { (*e1)->d_name, static_cast<storage::type>((*e1)->d_type), (*e1)->d_ino },
                               { (*e2)->d_name, static_cast<storage::type>((*e2)->d_type), (*e2)->d_ino } );
        }
    );
    if(n>=0)
    {
        storage::entries entries;

        for(dirent** name= names; n; ++name, --n)
        {
            entries.push_back({ (*name)->d_name, static_cast<storage::type>((*name)->d_type), (*name)->d_ino });
            free(*name);
        }

        free(names);
        return entries;
    }
    else
    {
        int e= errno;
        free(names);

        errno=e;
        throw error();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
