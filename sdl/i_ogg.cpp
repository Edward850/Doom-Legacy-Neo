// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: mserv.c,v 1.33 2003/06/05 20:34:48 hurdler Exp $
//
// Copyright (C) 2026 Edward Richardson
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Music module for ogg vorbis/opus music playback
//
//-----------------------------------------------------------------------------

#include "i_musicmodules.h"
#include <vorbis/vorbisfile.h>
#include <opusfile.h>
#include <thread>
#include <atomic>
#include <vector>

struct MemoryBuffer
{
    const uint8_t* data;
    size_t size;
    size_t offset;
};

static size_t vorbis_mem_read(void* ptr, size_t size, size_t nmemb, void* src)
{
    auto* buf = static_cast<MemoryBuffer*>(src);
    size_t bytes = size * nmemb;
    if (buf->offset + bytes > buf->size) bytes = buf->size - buf->offset;
    std::memcpy(ptr, buf->data + buf->offset, bytes);
    buf->offset += bytes;
    return bytes / size;
}
static int vorbis_mem_seek(void* src, ogg_int64_t offset, int whence)
{
    auto* buf = static_cast<MemoryBuffer*>(src);
    if (whence == SEEK_SET) buf->offset = offset;
    else if (whence == SEEK_CUR) buf->offset += offset;
    else if (whence == SEEK_END) buf->offset = buf->size + offset;
    if (buf->offset > buf->size) return -1;
    return 0;
}
static long vorbis_mem_tell(void* src)
{
    return static_cast<MemoryBuffer*>(src)->offset;
}
static long long opus_mem_tell(void* src)
{
    return static_cast<MemoryBuffer*>(src)->offset;
}
static int opus_mem_read(void* src, unsigned char* ptr, int nbytes)
{
    auto* buf = static_cast<MemoryBuffer*>(src);
    if (buf->offset + nbytes > buf->size) nbytes = static_cast<int>(buf->size - buf->offset);
    std::memcpy(ptr, buf->data + buf->offset, nbytes);
    buf->offset += nbytes;
    return nbytes;
}

class oggMusic final : public musicModule
{
private:
    enum class Codec { Unknown, Vorbis, Opus };
    Codec codec = Codec::Unknown;
    MemoryBuffer buf;
    OggVorbis_File vf{};
    OggOpusFile* of = nullptr;
    bool looping = false;
    size_t rate = 0;

    void Shutdown()
    {
        if (of)
        {
            op_free(of);
            of = nullptr;
        }
        if (codec == Codec::Vorbis)
        {
            ov_clear(&vf);
        }
    }

public:
    oggMusic(const void* data, size_t length)
    {
        if (length > 36 && std::memcmp(static_cast<const uint8_t*>(data) + 28, "OpusHead", 8) == 0) 
        {
            codec = Codec::Opus;
        }
        else
        {
            codec = Codec::Vorbis;
        }

        buf = { static_cast<const uint8_t*>(data), length, 0 };

        if (codec == Codec::Vorbis)
        {
            ov_callbacks vorbisCb{ vorbis_mem_read, vorbis_mem_seek, nullptr, vorbis_mem_tell };
            if (ov_open_callbacks(&buf, &vf, nullptr, 0, vorbisCb) != 0)
            {
                codec = Codec::Unknown;
            }
            else
            {
                vorbis_info* vi = ov_info(&vf, -1);
                if (vi)
                {
                    rate = vi->rate;
                }
            }
        }
        else if (codec == Codec::Opus)
        {
            OpusFileCallbacks opusCb{ opus_mem_read, vorbis_mem_seek, opus_mem_tell, nullptr };
            int error;
            of = op_open_callbacks(&buf, &opusCb, nullptr, 0, &error);
            if (!of)
            {
                codec = Codec::Unknown;
            }
            else
            {
                rate = 48000; // Opus has a fixed sample rate of 48kHz
            }
        }
    }

    ~oggMusic()
    {
        Shutdown();
    }

    virtual bool GenerateAudioData(void* outputBuffer, size_t outputBufferSize, const size_t channelCount) override
    {
        // Loop until we fill the output buffer
        size_t totalRead = 0;
        while (totalRead < outputBufferSize)
        {
            int64_t readFrames;
            if (codec == Codec::Vorbis)
            {
                readFrames = ov_read(&vf, static_cast<char*>(outputBuffer) + totalRead, static_cast<int>(outputBufferSize - totalRead), 0, 2, 1, nullptr);
            }
            else if (codec == Codec::Opus)
            {
                readFrames = op_read(of, static_cast<opus_int16*>(outputBuffer) + totalRead / sizeof(opus_int16), static_cast<int>((outputBufferSize - totalRead) / sizeof(opus_int16)), nullptr);
            }
            else
            {
                break; // Unknown codec
            }
            if (readFrames <= 0)
            {
                if (looping)
                {
                    if (codec == Codec::Vorbis)
                    {
                        ov_pcm_seek(&vf, 0);
                    }
                    else if (codec == Codec::Opus)
                    {
                        op_pcm_seek(of, 0);
                    }
                    continue;
                }
                else
                {
                    break;
                }
            }
            totalRead += readFrames;
        }
        return true;
    }

    virtual void SetLooping(bool loop) override
    {
        looping = loop;
    }

    virtual void StartSong() override
    {
        if (codec == Codec::Vorbis)
        {

        }
        else if (codec == Codec::Opus)
        {
        }
    }

    virtual void SetGain(double gain) override
    {
    }

    virtual const size_t GetSampleRate() const override
    {
        return rate;
    }
};

class oggMusicRegister final : public musicModuleRegister
{
public:
    oggMusicRegister()
    {
        RegisterModule(this);
    }

    virtual bool CheckCompatibleTrack(const void* data, size_t length) override
    {
        // Detect vorbis/opus by checking the magic numbers in the file header
        if (length >= 4)
        {
            const uint8_t* bytes = static_cast<const uint8_t*>(data);
            if (!memcmp(bytes, "OggS", 4))
            {
                return true; // Ogg container detected
            }
        }
        return false;
    }

    virtual musicModule* CreateModuleInstance(const void* data, size_t length) override
    {
        return new oggMusic(data, length);
    }
};

static oggMusicRegister oggMusicRegisterInstance;
