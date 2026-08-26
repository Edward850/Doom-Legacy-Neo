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
//      Music module for dr_mp3 music playback
//
//-----------------------------------------------------------------------------

#include "i_musicmodules.h"
#define DR_MP3_IMPLEMENTATION
#include <../dr_libs/dr_mp3.h>
#include <thread>
#include <atomic>
#include <vector>

class drMp3Music final : public musicModule
{
private:
    drmp3 mp3;
    bool looping = false;
    bool loaded = false;

    void Shutdown()
    {
        loaded = false;
    }

public:
    drMp3Music(const void* data, size_t length)
    {
        loaded = drmp3_init_memory(&mp3, data, length, nullptr);
    }

    ~drMp3Music()
    {
        Shutdown();
    }

    virtual bool GenerateAudioData(void* outputBuffer, size_t outputBufferSize, const size_t channelCount) override
    {
        if (loaded)
        {
            const uint64_t framesToRead = outputBufferSize / (channelCount * sizeof(int16_t));
            uint64_t readFrames = drmp3_read_pcm_frames_s16(&mp3, framesToRead, (int16_t*)outputBuffer);
            if (readFrames < framesToRead)
            {
                if(looping)
                {
                    drmp3_seek_to_pcm_frame(&mp3, 0);
                    // Read the remaining frames after looping
                    readFrames += drmp3_read_pcm_frames_s16(&mp3, framesToRead - readFrames, (int16_t*)outputBuffer + readFrames * channelCount);
                }
                else
                {
                    // Fill the rest of the buffer with silence if not looping
                    std::memset((int16_t*)outputBuffer + readFrames * channelCount, 0, (framesToRead - readFrames) * channelCount * sizeof(int16_t));
                    Shutdown();
                }
            }
            return readFrames > 0;
        }
        else
        {
            // Fill the buffer with silence if mp3 is not loaded
            std::memset(outputBuffer, 0, outputBufferSize);
        }
        return true;
    }

    virtual void SetLooping(bool loop) override
    {
        looping = loop;
    }

    virtual void StartSong() override
    {
        if(loaded)
        {
            drmp3_seek_to_pcm_frame(&mp3, 0);
        }
    }

    virtual void SetGain(double gain) override
    {
    }

    virtual const size_t GetSampleRate() const override
    {
        return 44100;
    }
};

class drMp3MusicRegister final : public musicModuleRegister
{
public:
    drMp3MusicRegister()
    {
        RegisterModule(this);
    }

    virtual bool CheckCompatibleTrack(const void* data, size_t length) override
    {
        // Look at the first 3 bytes for the "ID3" tag or check for MP3 frame sync bits
        if (length >= 3 && !memcmp(data, "ID3", 3))
        {
            return true;
        }
        // Maybe not the frame sync bits, there could be false postives with other formats.
        /*else if (length >= 2 && ((reinterpret_cast<const uint8_t*>(data)[0] & 0xFF) == 0xFF) && ((reinterpret_cast<const uint8_t*>(data)[1] & 0xE0) == 0xE0))
        {
            return true;
        }*/
        return false;
    }

    virtual musicModule* CreateModuleInstance(const void* data, size_t length) override
    {
        return new drMp3Music(data, length);
    }
};

static drMp3MusicRegister drMp3MusicRegisterInstance;
