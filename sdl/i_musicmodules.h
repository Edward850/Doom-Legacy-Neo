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
//      Base module for each seperate music module.
//
//-----------------------------------------------------------------------------

#pragma once
#include <vector>

class musicModule 
{
public:
    virtual bool GenerateAudioData(void* outputBuffer, size_t outputBufferSize, const size_t channelCount) = 0;
    virtual void SetLooping(bool loop) = 0;
    virtual void StartSong() = 0;
    virtual void SetGain(double gain) = 0;
    virtual const size_t GetSampleRate() const = 0;

    virtual ~musicModule() = default;
};

class musicModuleRegister
{
private:
    static std::vector<musicModuleRegister*>& GetRegisteredMusicModules();
protected:
    static void RegisterModule(musicModuleRegister* module)
    {
        GetRegisteredMusicModules().push_back(module);
    }
public:
    virtual bool CheckCompatibleTrack(const void* data, size_t length) = 0;
    virtual musicModule* CreateModuleInstance(const void* data, size_t length) = 0;
    virtual void RecieveLumpData(const char* name, const void* data, size_t length) {};
    static musicModule* GetCompatibleModule(const void* data, size_t length)
    {
        for (auto& module : GetRegisteredMusicModules())
        {
            if (module->CheckCompatibleTrack(data, length))
            {
                return module->CreateModuleInstance(data, length);
            }
        }
        return nullptr;
    }
    static void SendLumpData(const char* name, const void* data, size_t length)
    {
        for (auto& module : GetRegisteredMusicModules())
        {
            module->RecieveLumpData(name, data, length);
        }
    }
};
