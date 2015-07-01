///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ALSA_DEVICE_HPP
#define ALSA_DEVICE_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "alsa/alsa_error.hpp"
#include "enum.hpp"

#include <chrono>
#include <string>
#include <utility>

#include <alsa/asoundlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace alsa
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief  type of stream (capture/playback)
///
enum class stream
{
    playback = SND_PCM_STREAM_PLAYBACK,
    capture  = SND_PCM_STREAM_CAPTURE,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief  options
///
enum class opt
{
    none      = 0x00,
    non_block = 0x01,
    resample  = 0x02, // software resample
};
DECLARE_OPERATOR(opt)

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace sample
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief  capture/playback sample format
///
enum format
{
    unknown   = SND_PCM_FORMAT_UNKNOWN,            // Unknown
    s8        = SND_PCM_FORMAT_S8,                 // Signed 8 bit
    u8        = SND_PCM_FORMAT_U8,                 // Unsigned 8 bit
    s16_le    = SND_PCM_FORMAT_S16_LE,             // Signed 16 bit Little Endian
    s16_be    = SND_PCM_FORMAT_S16_BE,             // Signed 16 bit Big Endian
    u16_le    = SND_PCM_FORMAT_U16_LE,             // Unsigned 16 bit Little Endian
    u16_be    = SND_PCM_FORMAT_U16_BE,             // Unsigned 16 bit Big Endian
    s24_le    = SND_PCM_FORMAT_S24_LE,             // Signed 24 bit Little Endian using low three bytes in 32-bit word
    s24_be    = SND_PCM_FORMAT_S24_BE,             // Signed 24 bit Big Endian using low three bytes in 32-bit word
    u24_le    = SND_PCM_FORMAT_U24_LE,             // Unsigned 24 bit Little Endian using low three bytes in 32-bit word
    u24_be    = SND_PCM_FORMAT_U24_BE,             // Unsigned 24 bit Big Endian using low three bytes in 32-bit word
    s32_le    = SND_PCM_FORMAT_S32_LE,             // Signed 32 bit Little Endian
    s32_be    = SND_PCM_FORMAT_S32_BE,             // Signed 32 bit Big Endian
    u32_le    = SND_PCM_FORMAT_U32_LE,             // Unsigned 32 bit Little Endian
    u32_be    = SND_PCM_FORMAT_U32_BE,             // Unsigned 32 bit Big Endian
    f32_le    = SND_PCM_FORMAT_FLOAT_LE,           // Float 32 bit Little Endian, Range -1.0 to 1.0
    f32_be    = SND_PCM_FORMAT_FLOAT_BE,           // Float 32 bit Big Endian, Range -1.0 to 1.0
    f64_le    = SND_PCM_FORMAT_FLOAT64_LE,         // Float 64 bit Little Endian, Range -1.0 to 1.0
    f64_be    = SND_PCM_FORMAT_FLOAT64_BE,         // Float 64 bit Big Endian, Range -1.0 to 1.0
    iec958_le = SND_PCM_FORMAT_IEC958_SUBFRAME_LE, // IEC-958 Little Endian
    iec958_be = SND_PCM_FORMAT_IEC958_SUBFRAME_BE, // IEC-958 Big Endian
    mu_law    = SND_PCM_FORMAT_MU_LAW,             // Mu-Law
    a_law     = SND_PCM_FORMAT_A_LAW,              // A-Law
    ima_adpcm = SND_PCM_FORMAT_IMA_ADPCM,          // Ima-ADPCM
    mpeg      = SND_PCM_FORMAT_MPEG,               // MPEG
    gsm       = SND_PCM_FORMAT_GSM,                // GSM
    special   = SND_PCM_FORMAT_SPECIAL,            // Special
    s24_3le   = SND_PCM_FORMAT_S24_3LE,            // Signed 24bit Little Endian in 3bytes format
    s24_3be   = SND_PCM_FORMAT_S24_3BE,            // Signed 24bit Big Endian in 3bytes format
    u24_3le   = SND_PCM_FORMAT_U24_3LE,            // Unsigned 24bit Little Endian in 3bytes format
    u24_3be   = SND_PCM_FORMAT_U24_3BE,            // Unsigned 24bit Big Endian in 3bytes format
    s20_3le   = SND_PCM_FORMAT_S20_3LE,            // Signed 20bit Little Endian in 3bytes format
    s20_3be   = SND_PCM_FORMAT_S20_3BE,            // Signed 20bit Big Endian in 3bytes format
    u20_3le   = SND_PCM_FORMAT_U20_3LE,            // Unsigned 20bit Little Endian in 3bytes format
    u20_3be   = SND_PCM_FORMAT_U20_3BE,            // Unsigned 20bit Big Endian in 3bytes format
    s18_3le   = SND_PCM_FORMAT_S18_3LE,            // Signed 18bit Little Endian in 3bytes format
    s18_3be   = SND_PCM_FORMAT_S18_3BE,            // Signed 18bit Big Endian in 3bytes format
    u18_3le   = SND_PCM_FORMAT_U18_3LE,            // Unsigned 18bit Little Endian in 3bytes format
    u18_3be   = SND_PCM_FORMAT_U18_3BE,            // Unsigned 18bit Big Endian in 3bytes format
    s16       = SND_PCM_FORMAT_S16,                // Signed 16 bit CPU endian
    u16       = SND_PCM_FORMAT_U16,                // Unsigned 16 bit CPU endian
    s24       = SND_PCM_FORMAT_S24,                // Signed 24 bit CPU endian
    u24       = SND_PCM_FORMAT_U24,                // Unsigned 24 bit CPU endian
    s32       = SND_PCM_FORMAT_S32,                // Signed 32 bit CPU endian
    u32       = SND_PCM_FORMAT_U32,                // Unsigned 32 bit CPU endian
    f32       = SND_PCM_FORMAT_FLOAT,              // Float 32 bit CPU endian
    f64       = SND_PCM_FORMAT_FLOAT64,            // Float 64 bit CPU endian
    iec958    = SND_PCM_FORMAT_IEC958_SUBFRAME,    // iec-958 cpu endian
};

////////////////////
/// \brief  name get sample format name
/// \param  format sample format
/// \return sample format name
///
std::string name(alsa::sample::format format) noexcept;

////////////////////
/// \brief  description get sample format description
/// \param  format sample format
/// \return sample format descrtiption
///
std::string description(alsa::sample::format format) noexcept;

////////////////////
/// \brief  bits get sample size in bits
/// \param  format sample format
/// \return sample size in bits
///
int bits(alsa::sample::format format);

////////////////////
/// \brief  size get physical sample size in bytes
/// \param  format sample format
/// \return physical sample size in bytes
///
int size(alsa::sample::format format);

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef snd_pcm_uframes_t frames;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief  ALSA capture/playback device
///
/// This class handles capture/playback with ALSA devices.
///
class device
{
public:
    device() noexcept = default;
    device(const device&) = delete;
    device(device&& x) noexcept { swap(x); }

    ~device() { close(); }

    ////////////////////
    /// \brief  device create ALSA device
    /// \param  name ALSA device name
    /// \param  stream type of stream (playback/capture)
    /// \param  format sample format
    /// \param  channels number of channels
    /// \param  rate sample rate
    /// \param  opt options
    /// \param  latency latency
    /// \param  periods number of periods
    ///
    template<typename Rep, typename Period>
    device(const std::string& name, alsa::stream stream, alsa::sample::format format, int channels, int rate, alsa::opt opt, const std::chrono::duration<Rep, Period>& latency, int periods = 8) :
        device(name, stream, format, channels, rate, opt, std::chrono::duration_cast<std::chrono::microseconds>(latency).count(), periods)
    { }

    ////////////////////
    /// \brief  close close ALSA device
    ///
    void close() noexcept;

    ////////////////////
    /// \brief  is_open check if ALSA device is open
    /// \return open/closed
    ///
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
    bool run() noexcept;
    bool stop(bool drain = false) noexcept;
    bool running() const noexcept;

    bool can_pause() const noexcept;
    bool pause() noexcept;
    bool unpause() noexcept;
    bool paused() const noexcept;

    ////////////////////
    /// \brief  period get period size in frames
    /// \return period size in frames
    ///
    alsa::frames period();

    ////////////////////
    /// \brief  read read (capture) data from ALSA device
    /// \param  buffer buffer to read data into
    /// \param  frames buffer size in frames
    /// \return number of frames read (captured)
    ///
    alsa::frames read(void* buffer, alsa::frames frames);

    ////////////////////
    /// \brief  write write (play back) data to ALSA device
    /// \param  buffer buffer to write data from
    /// \param  frames buffer size in frames
    /// \return number of frames written (played back)
    ///
    alsa::frames write(void* buffer, alsa::frames frames);

    ////////////////////
    /// \brief  recover recover ALSA device
    /// \param  e ALSA error
    /// \return whether the device was recovered
    ///
    bool recover(const alsa::alsa_error& e) noexcept;

protected:
    snd_pcm_t* _M_pcm = nullptr;

    device(const std::string& name, alsa::stream, alsa::sample::format, int channels, int rate, alsa::opt, int microseconds, int periods);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ALSA_DEVICE_HPP
