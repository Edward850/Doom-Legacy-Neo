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
//      Music module for LibOpenMPT music playback
//
//-----------------------------------------------------------------------------

#include "i_musicmodules.h"
#include <libopenmpt/libopenmpt.h>
#include <libopenmpt/libopenmpt.hpp>
#include <thread>
#include <atomic>
#include <vector>

class openMptMusic final : public musicModule
{
private:
    openmpt_module* mod = nullptr;

    void Shutdown()
    {
        if(mod)
        {
            openmpt_module_destroy(mod);
            mod = nullptr;
        }
    }

public:
    openMptMusic(const void* data, size_t length)
    {
        mod = openmpt_module_create_from_memory(data, length, nullptr, nullptr, nullptr);
        if (!mod)
        {
            Shutdown();
            return;
        }
    }

    ~openMptMusic()
    {
        Shutdown();
    }

    virtual bool GenerateAudioData(void* outputBuffer, size_t outputBufferSize, const size_t channelCount) override
    {
        if(mod)
        {
            size_t frames = outputBufferSize / (channelCount * sizeof(int16_t));
            size_t readFrames = openmpt_module_read_interleaved_stereo(mod, 44100, frames, (int16_t*)outputBuffer);
            return readFrames > 0;
        }
        return true;
    }

    virtual void SetLooping(bool loop) override
    {
        if (mod)
        {
            openmpt_module_set_repeat_count(mod, loop ? -1 : 0);
        }
    }

    virtual void StartSong() override
    {
        if (mod)
        {
            //openmpt_module_restart(mod);
        }
    }

    virtual void SetGain(double gain) override
    {
        if (mod)
        {
            //openmpt_module_set_gain(mod, gain);
        }
    }

    virtual const size_t GetSampleRate() const override
    {
        return 44100;
    }
};

class openMptMusicRegister final : public musicModuleRegister
{
public:
    openMptMusicRegister()
    {
        RegisterModule(this);
    }

    virtual bool CheckCompatibleTrack(const void* data, size_t length) override
    {
        if (openmpt::probe_file_header(openmpt::probe_file_header_flags_default2, (const uint8_t*)data, length) == openmpt::probe_file_header_result_success)
        {
            return true;
        }
        return false;
    }

    virtual musicModule* CreateModuleInstance(const void* data, size_t length) override
    {
        return new openMptMusic(data, length);
    }
};

static openMptMusicRegister openMptMusicRegisterInstance;
