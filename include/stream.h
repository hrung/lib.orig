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

    stream& operator<<(const stream& value) { return (*this) << value.buffer(); }

    std::string buffer() const { return str(); }
    void buffer(const std::string& value) { str(value); }

    operator std::string() const { return buffer(); }
    const char* data() const { return buffer().data(); }

    void clear() { buffer(std::string()); }

#ifdef QT_CORE_LIB
    stream& operator<<(const QString& value) { return (*this) << value.toStdString(); }
    operator QString() { return QString::fromStdString(buffer()); }
#endif
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
