///////////////////////////////////////////////////////////////////////////////////////////////////
#include "device.h"
#include "alsa_error.h"

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
device::device(const std::string& name, alsa::stream stream, alsa::mode mode)
try
{
    int code = snd_pcm_open(&_M_pcm, name.data(), static_cast<snd_pcm_stream_t>(stream), static_cast<int>(mode));
    if(code) throw alsa_error(code, "snd_pcm_open");

    code = snd_pcm_hw_params_malloc(&_M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_malloc");

    code = snd_pcm_hw_params_any(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_any");
}
catch(...)
{
    close();
    throw;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::close() noexcept
{
    if(_M_params)
    {
        snd_pcm_hw_params_free(_M_params);
        _M_params = nullptr;
    }
    if(_M_pcm)
    {
        snd_pcm_close(_M_pcm);
        _M_pcm = nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_access(alsa::access access)
{
    int code = snd_pcm_hw_params_set_access(_M_pcm, _M_params, static_cast<snd_pcm_access_t>(access));
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_access");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::access device::access()
{
    snd_pcm_access_t access;

    int code = snd_pcm_hw_params_get_access(_M_params, &access);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_access");

    return static_cast<alsa::access>(access);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_format(alsa::format format)
{
    int code = snd_pcm_hw_params_set_format(_M_pcm, _M_params, static_cast<snd_pcm_format_t>(format));
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_format");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::format device::format()
{
    snd_pcm_format_t format;

    int code = snd_pcm_hw_params_get_format(_M_params, &format);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_format");

    return static_cast<alsa::format>(format);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_rate(alsa::rate* rate)
{
    int code = snd_pcm_hw_params_set_rate_near(_M_pcm, _M_params, rate, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_rate_near");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::rate device::rate()
{
    alsa::rate rate;

    int code = snd_pcm_hw_params_get_rate(_M_params, &rate, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_rate");

    return rate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_channels(alsa::channels channels)
{
    int code = snd_pcm_hw_params_set_channels(_M_pcm, _M_params, channels);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_channels");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::channels device::channels()
{
    alsa::channels channels;

    int code = snd_pcm_hw_params_get_channels(_M_params, &channels);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_channels");

    return channels;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_resample(bool resample)
{
    int code = snd_pcm_hw_params_set_rate_resample(_M_pcm, _M_params, resample);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_rate_resample");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::resample()
{
    unsigned resample;

    int code = snd_pcm_hw_params_get_rate_resample(_M_pcm, _M_params, &resample);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_rate_resample");

    return resample;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_period(alsa::frames* frames)
{
    int code = snd_pcm_hw_params_set_period_size_near(_M_pcm, _M_params, frames, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_period_size_near");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
alsa::frames device::period()
{
    alsa::frames frames;

    int code = snd_pcm_hw_params_get_period_size(_M_params, &frames, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_period_size");

    return frames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::set_periods(unsigned* periods)
{
    int code = snd_pcm_hw_params_set_periods_near(_M_pcm, _M_params, periods, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_set_periods_near");

    code = snd_pcm_hw_params(_M_pcm, _M_params);
    if(code) throw alsa_error(code, "snd_pcm_hw_params");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned device::periods()
{
    unsigned periods;

    int code = snd_pcm_hw_params_get_periods(_M_params, &periods, nullptr);
    if(code) throw alsa_error(code, "snd_pcm_hw_params_get_periods");

    return periods;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int device::size(alsa::frames frames) noexcept
{
   return snd_pcm_frames_to_bytes(_M_pcm, frames);
}

alsa::frames device::frames(int count) noexcept
{
    return snd_pcm_bytes_to_frames(_M_pcm, count);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
int device::read(void* buffer, unsigned frames)
{
    int n = snd_pcm_readi(_M_pcm, buffer, frames);
    if(n < 0) throw alsa_error(n, "snd_pcm_readi");

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int device::write(void* buffer, unsigned frames)
{
    int n = snd_pcm_writei(_M_pcm, buffer, frames);
    if(n < 0) throw alsa_error(n, "snd_pcm_writei");

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
}
