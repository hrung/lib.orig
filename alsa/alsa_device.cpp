///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "alsa/alsa_device.hpp"

#include <climits>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace alsa
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
const std::error_category& alsa_category()
{
    static class alsa_category instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace sample
{

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string name(alsa::sample::format format) noexcept
{
    return snd_pcm_format_name(static_cast<snd_pcm_format_t>(format));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string description(alsa::sample::format format) noexcept
{
    return snd_pcm_format_description(static_cast<snd_pcm_format_t>(format));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int bits(alsa::sample::format format)
{
    auto code = snd_pcm_format_width(static_cast<snd_pcm_format_t>(format));
    if(code < 0) throw alsa_error(code, "snd_pcm_format_width");

    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int size(alsa::sample::format format)
{
    auto code = snd_pcm_format_physical_width(static_cast<snd_pcm_format_t>(format));
    if(code < 0) throw alsa_error(code, "snd_pcm_format_physical_width");

    return code / CHAR_BIT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
device::device(const std::string& name, alsa::stream stream, alsa::sample::format format, int channels, int rate, alsa::opt opt, int microseconds, int periods)
{
    int mode = opt && alsa::opt::non_block ? SND_PCM_NONBLOCK : 0;

    int code = snd_pcm_open(&_M_pcm, name.data(), static_cast<snd_pcm_stream_t>(stream), mode);
    if(code) throw alsa_error(code, "snd_pcm_open");

    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);

    code = snd_pcm_hw_params_any(_M_pcm, params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_any");

    code = snd_pcm_hw_params_set_rate_resample(_M_pcm, params, opt && alsa::opt::resample);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_rate_resample");

    code = snd_pcm_hw_params_set_access(_M_pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_access");

    code = snd_pcm_hw_params_set_format(_M_pcm, params, static_cast<snd_pcm_format_t>(format));
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_format");

    code = snd_pcm_hw_params_set_channels(_M_pcm, params, channels);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_channels");

    unsigned near = rate;
    code = snd_pcm_hw_params_set_rate_near(_M_pcm, params, &near, 0);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_rate_near");
    if(rate != (int)near) throw alsa_error(alsa::errc::invalid_argument, "snd_pcm_hw_params_set_rate_near");

    unsigned period = microseconds / periods;
    code = snd_pcm_hw_params_set_period_time_near(_M_pcm, params, &period, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_period_time_near");

    alsa::frames buffer = 0;
    code = snd_pcm_hw_params_get_period_size(params, &buffer, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_period_size");

    buffer *= periods;
    code = snd_pcm_hw_params_set_buffer_size_near(_M_pcm, params, &buffer);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_buffer_size_near");

    code = snd_pcm_hw_params(_M_pcm, params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::close() noexcept
{
    if(_M_pcm)
    {
        snd_pcm_close(_M_pcm);
        _M_pcm = nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::run() noexcept
{
    if(snd_pcm_prepare(_M_pcm))
        return false;
    else return !snd_pcm_start(_M_pcm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::stop(bool drain) noexcept
{
    return !(drain ? snd_pcm_drain : snd_pcm_drop)(_M_pcm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::running() const noexcept
{
    return snd_pcm_state(_M_pcm) == SND_PCM_STATE_RUNNING;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::can_pause() const noexcept
{
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);

    if(snd_pcm_hw_params_current(_M_pcm, params))
        return false;
    else return snd_pcm_hw_params_can_pause(params);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::pause() noexcept
{
    return !snd_pcm_pause(_M_pcm, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::unpause() noexcept
{
    return !snd_pcm_pause(_M_pcm, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::paused() const noexcept
{
    return snd_pcm_state(_M_pcm) == SND_PCM_STATE_PAUSED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::frames device::period()
{
    alsa::frames buffer, period;

    int code = snd_pcm_get_params(_M_pcm, &buffer, &period);
    if(code) throw alsa_error(code, "snd_pcm_get_params");

    return period;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::frames device::read(void* buffer, alsa::frames frames)
{
    int code = snd_pcm_readi(_M_pcm, buffer, frames);
    if(code < 0) throw alsa_error(code, "snd_pcm_readi");

    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::frames device::write(void* buffer, alsa::frames frames)
{
    int code = snd_pcm_writei(_M_pcm, buffer, frames);
    if(code < 0) throw alsa_error(code, "snd_pcm_writei");

    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::recover(const alsa::alsa_error& e) noexcept
{
    return !snd_pcm_recover(_M_pcm, e.code().value(), true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
