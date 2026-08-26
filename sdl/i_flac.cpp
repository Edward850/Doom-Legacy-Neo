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
//      Music module for dr_flac music playback
//
//-----------------------------------------------------------------------------

#include "i_musicmodules.h"
#define DR_FLAC_IMPLEMENTATION
#include <../dr_libs/dr_flac.h>
#include <thread>
#include <atomic>
#include <vector>

class drFlacMusic final : public musicModule
{
private:
    drflac* flac = nullptr;
    bool looping = false;

    void Shutdown()
    {
        if (flac)
        {
            drflac_close(flac);
            flac = nullptr;
        }
    }

public:
    drFlacMusic(const void* data, size_t length)
    {
        flac = drflac_open_memory(data, length, nullptr);
    }

    ~drFlacMusic()
    {
        Shutdown();
    }

    virtual bool GenerateAudioData(void* outputBuffer, size_t outputBufferSize, const size_t channelCount) override
    {
        if (flac)
        {
            const uint64_t framesToRead = outputBufferSize / (channelCount * sizeof(int16_t));
            uint64_t readFrames = drflac_read_pcm_frames_s16(flac, framesToRead, (int16_t*)outputBuffer);
            if (readFrames < framesToRead)
            {
                if(looping)
                {
                    drflac_seek_to_pcm_frame(flac, 0);
                    // Read the remaining frames after looping
                    readFrames += drflac_read_pcm_frames_s16(flac, framesToRead - readFrames, (int16_t*)outputBuffer + readFrames * channelCount);
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
            // Fill the buffer with silence if flac is not loaded
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
        if (flac)
        {
            drflac_seek_to_pcm_frame(flac, 0);
        }
    }

    virtual void SetGain(double gain) override
    {}

    virtual const size_t GetSampleRate() const override
    {
        return 44100;
    }
};

class drFlacMusicRegister final : public musicModuleRegister
{
public:
    drFlacMusicRegister()
    {
        RegisterModule(this);
    }

    virtual bool CheckCompatibleTrack(const void* data, size_t length) override
    {
        // Check for the FLAC signature "fLaC" at the beginning of the data
        if (length >= 4 && std::memcmp(data, "fLaC", 4) == 0)
        {
            return true;
        }
        return false;
    }

    virtual musicModule* CreateModuleInstance(const void* data, size_t length) override
    {
        return new drFlacMusic(data, length);
    }
};

static drFlacMusicRegister drFlacMusicRegisterInstance;
