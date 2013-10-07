///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef STREAM_H
#define STREAM_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <sstream>

#ifdef QT_CORE_LIB
#  include <QString>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
class stream: public std::ostringstream
{
public:
    stream() = default;

    template<typename T>
    explicit stream(const T& value) { (*this) << value; }

    template<typename T>
    stream& operator<<(const T& value)
    {
        *static_cast<std::ostringstream*>(this) << value;
        return (*this);
    }

    stream& operator<<(const stream& value) { return (*this) << value.get(); }

    std::string get() const { return str(); }
    void set(const std::string& value) { str(value); }

    operator std::string() const { return get(); }
    const char* data() const { return get().data(); }

    void clear() { set(std::string()); }

#ifdef QT_CORE_LIB
    stream& operator<<(const QString& value) { return (*this) << value.toStdString(); }
    operator QString() { return QString::fromStdString(get()); }
#endif
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
