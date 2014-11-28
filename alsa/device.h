///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DEVICE_H
#define DEVICE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "enum.h"

#include <string>
#include <utility>

#include <alsa/asoundlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace alsa
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class stream
{
    playback    = SND_PCM_STREAM_PLAYBACK,
    capture     = SND_PCM_STREAM_CAPTURE,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class mode
{
    normal      = 0,
    non_block   = SND_PCM_NONBLOCK,
    async       = SND_PCM_ASYNC,
};
ENUM_OPERATOR(mode)

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class format
{
    unknown     = SND_PCM_FORMAT_UNKNOWN,             // Unknown
    s8          = SND_PCM_FORMAT_S8,                  // Signed 8 bit
    u8          = SND_PCM_FORMAT_U8,                  // Unsigned 8 bit
    s16_le      = SND_PCM_FORMAT_S16_LE,              // Signed 16 bit Little Endian
    s16_be      = SND_PCM_FORMAT_S16_BE,              // Signed 16 bit Big Endian
    u16_le      = SND_PCM_FORMAT_U16_LE,              // Unsigned 16 bit Little Endian
    u16_be      = SND_PCM_FORMAT_U16_BE,              // Unsigned 16 bit Big Endian
    s24_le      = SND_PCM_FORMAT_S24_LE,              // Signed 24 bit Little Endian using low three bytes in 32-bit word
    s24_be      = SND_PCM_FORMAT_S24_BE,              // Signed 24 bit Big Endian using low three bytes in 32-bit word
    u24_le      = SND_PCM_FORMAT_U24_LE,              // Unsigned 24 bit Little Endian using low three bytes in 32-bit word
    u24_be      = SND_PCM_FORMAT_U24_BE,              // Unsigned 24 bit Big Endian using low three bytes in 32-bit word
    s32_le      = SND_PCM_FORMAT_S32_LE,              // Signed 32 bit Little Endian
    s32_be      = SND_PCM_FORMAT_S32_BE,              // Signed 32 bit Big Endian
    u32_le      = SND_PCM_FORMAT_U32_LE,              // Unsigned 32 bit Little Endian
    u32_be      = SND_PCM_FORMAT_U32_BE,              // Unsigned 32 bit Big Endian
    f32_le      = SND_PCM_FORMAT_FLOAT_LE,            // Float 32 bit Little Endian, Range -1.0 to 1.0
    f32_be      = SND_PCM_FORMAT_FLOAT_BE,            // Float 32 bit Big Endian, Range -1.0 to 1.0
    f64_le      = SND_PCM_FORMAT_FLOAT64_LE,          // Float 64 bit Little Endian, Range -1.0 to 1.0
    f64_be      = SND_PCM_FORMAT_FLOAT64_BE,          // Float 64 bit Big Endian, Range -1.0 to 1.0
    iec958_le   = SND_PCM_FORMAT_IEC958_SUBFRAME_LE,  // IEC-958 Little Endian
    iec958_be   = SND_PCM_FORMAT_IEC958_SUBFRAME_BE,  // IEC-958 Big Endian
    mu_law      = SND_PCM_FORMAT_MU_LAW,              // Mu-Law
    a_law       = SND_PCM_FORMAT_A_LAW,               // A-Law
    ima_adpcm   = SND_PCM_FORMAT_IMA_ADPCM,           // Ima-ADPCM
    mpeg        = SND_PCM_FORMAT_MPEG,                // MPEG
    gsm         = SND_PCM_FORMAT_GSM,                 // GSM
    special     = SND_PCM_FORMAT_SPECIAL,             // Special
    s24_3le     = SND_PCM_FORMAT_S24_3LE,             // Signed 24bit Little Endian in 3bytes format
    s24_3be     = SND_PCM_FORMAT_S24_3BE,             // Signed 24bit Big Endian in 3bytes format
    u24_3le     = SND_PCM_FORMAT_U24_3LE,             // Unsigned 24bit Little Endian in 3bytes format
    u24_3be     = SND_PCM_FORMAT_U24_3BE,             // Unsigned 24bit Big Endian in 3bytes format
    s20_3le     = SND_PCM_FORMAT_S20_3LE,             // Signed 20bit Little Endian in 3bytes format
    s20_3be     = SND_PCM_FORMAT_S20_3BE,             // Signed 20bit Big Endian in 3bytes format
    u20_3le     = SND_PCM_FORMAT_U20_3LE,             // Unsigned 20bit Little Endian in 3bytes format
    u20_3be     = SND_PCM_FORMAT_U20_3BE,             // Unsigned 20bit Big Endian in 3bytes format
    s18_3le     = SND_PCM_FORMAT_S18_3LE,             // Signed 18bit Little Endian in 3bytes format
    s18_3be     = SND_PCM_FORMAT_S18_3BE,             // Signed 18bit Big Endian in 3bytes format
    u18_3le     = SND_PCM_FORMAT_U18_3LE,             // Unsigned 18bit Little Endian in 3bytes format
    u18_3be     = SND_PCM_FORMAT_U18_3BE,             // Unsigned 18bit Big Endian in 3bytes format
    s16         = SND_PCM_FORMAT_S16,                 // Signed 16 bit CPU endian
    u16         = SND_PCM_FORMAT_U16,                 // Unsigned 16 bit CPU endian
    s24         = SND_PCM_FORMAT_S24,                 // Signed 24 bit CPU endian
    u24         = SND_PCM_FORMAT_U24,                 // Unsigned 24 bit CPU endian
    s32         = SND_PCM_FORMAT_S32,                 // Signed 32 bit CPU endian
    u32         = SND_PCM_FORMAT_U32,                 // Unsigned 32 bit CPU endian
    f32         = SND_PCM_FORMAT_FLOAT,               // Float 32 bit CPU endian
    f64         = SND_PCM_FORMAT_FLOAT64,             // Float 64 bit CPU endian
    iec958      = SND_PCM_FORMAT_IEC958_SUBFRAME,     // iec-958 cpu endian
};

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class access
{
    mmap            = SND_PCM_ACCESS_MMAP_INTERLEAVED,
    mmap_non        = SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
    mmap_complex    = SND_PCM_ACCESS_MMAP_COMPLEX,

    read_write      = SND_PCM_ACCESS_RW_INTERLEAVED,
    read_write_non  = SND_PCM_ACCESS_RW_NONINTERLEAVED,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef snd_pcm_uframes_t frames;

///////////////////////////////////////////////////////////////////////////////////////////////////
class device
{
public:
    device() noexcept = default;
    device(const device&) = delete;
    device(device&& x) noexcept { swap(x); }

    ~device() { close(); }

    device(const std::string& name, alsa::stream stream, alsa::mode mode = alsa::mode::normal);

    void close() noexcept;
    bool is_open() const noexcept { return _M_pcm != nullptr; }

    device& operator=(const device&) = delete;
    device& operator=(device&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(device& x) noexcept
    {
        std::swap(_M_pcm, x._M_pcm);
    }

    ////////////////////
    void set(alsa::format, alsa::access, unsigned channels, unsigned rate, bool resample, unsigned latency);

    alsa::frames period();
    unsigned frame_size();

    alsa::frames read(void* buffer, alsa::frames);
    alsa::frames write(void* buffer, alsa::frames);
    void recover(int code, bool silent = true);

protected:
    snd_pcm_t* _M_pcm = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // DEVICE_H
