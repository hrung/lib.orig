///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ALSA_ERROR_HPP
#define ALSA_ERROR_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <cerrno>
#include <string>
#include <system_error>

#include <alsa/asoundlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace alsa
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief  ALSA errors
///
enum class errc
{
    interrupted                         = -EINTR,
    resource_unavailable_try_again      = -EAGAIN,
    no_such_device                      = -ENODEV,
    invalid_argument                    = -EINVAL,
    io_error                            = -EIO,
    inappropriate_io_control_operation  = -ENOTTY,
    broken_pipe                         = -EPIPE,

    function_not_supported              = -ENOSYS,
    file_descriptor_in_bad_state        = -EBADFD,
    streams_pipe_error                  = -ESTRPIPE,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class alsa_category: public std::error_category
{
public:
    const char* name() const noexcept override { return "alsa"; }
    std::string message(int ev) const override { return snd_strerror(ev); }
};

const std::error_category& alsa_category();

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::error_code make_error_code(alsa::errc e)
{ return std::error_code(static_cast<int>(e), alsa_category()); }

inline std::error_condition make_error_condition(alsa::errc e)
{ return std::error_condition(static_cast<int>(e), alsa_category()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class alsa_error: public std::system_error
{
public:
    alsa_error(int code): std::system_error(std::error_code(code, alsa::alsa_category())) { }
    alsa_error(int code, const std::string& message): std::system_error(std::error_code(code, alsa::alsa_category()), message) { }

    alsa_error(alsa::errc code): std::system_error(std::error_code(static_cast<int>(code), alsa::alsa_category())) { }
    alsa_error(alsa::errc code, const std::string& message): std::system_error(std::error_code(static_cast<int>(code), alsa::alsa_category()), message) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace std
{
    template<>
    struct is_error_code_enum<alsa::errc>: public true_type { };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ALSA_ERROR_HPP
