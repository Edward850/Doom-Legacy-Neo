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
//      Music module for YMFMIDI music playback
//
//-----------------------------------------------------------------------------

#include "i_musicmodules.h"
#include "../ymfmidi/src/player.h"
#include <thread>
#include <atomic>
#include <vector>

static OPLPlayer::ChipType chipType = OPLPlayer::ChipType::ChipOPL3;
static std::vector<uint8_t> patchData;

class ymfmMusic final : public musicModule
{
private:
    OPLPlayer* m_pPlayer;
    std::thread* m_thread;
    std::vector<std::vector<signed short>> m_threadSamples;
    int m_threadSampleIndex;
    std::atomic<int> m_sampleCountNeeded;
    std::atomic<int> m_sampleCountReady;
    std::atomic<bool> m_threadTerminate;
    int m_underrun;
    int m_samplePosition;
    int m_numChips;

    void Shutdown()
    {
        m_threadTerminate = true;
        if (m_thread)
        {
            m_thread->join();
            delete m_thread;
            m_thread = nullptr;
        }
        m_threadTerminate = false;

        if (m_pPlayer)
        {
            delete m_pPlayer;
            m_pPlayer = nullptr;
        }
    }

public:
    ymfmMusic(const void* data, size_t length)
    {
#if _DEBUG
        m_numChips = 1;
#else
        m_numChips = 8;
#endif
        m_threadSamples.clear();
        m_sampleCountNeeded = 0;
        m_sampleCountReady = 0;
        m_threadSampleIndex = 0;
        m_underrun = -1;
        m_samplePosition = 0;
        m_threadTerminate = false;
        m_thread = nullptr;
        m_pPlayer = new OPLPlayer(m_numChips, chipType);
        if (!m_pPlayer->loadPatches(patchData.data(), patchData.size()))
        {
            Shutdown();
            return;
        }
        if (!m_pPlayer->loadSequence((const uint8_t*)data, length))
        {
            Shutdown();
            return;
        }
        m_pPlayer->setStereo(true);
        m_pPlayer->setGain(15.0);
    }

    ~ymfmMusic()
    {
        Shutdown();
    }

    virtual bool GenerateAudioData(void* outputBuffer, size_t outputBufferSize, const size_t channelCount) override
    {
        if (m_pPlayer)
        {
            m_pPlayer->generate((signed short*)outputBuffer, outputBufferSize / sizeof(signed short) / channelCount);
        }
        return true;
    }

    virtual void SetLooping(bool loop) override
    {
        if (m_pPlayer)
        {
            m_pPlayer->setLoop(loop);
        }
    }

    virtual void StartSong() override
    {
        if (m_pPlayer)
        {
            m_pPlayer->reset();
        }
    }

    virtual void SetGain(double gain) override
    {
        if (m_pPlayer)
        {
            m_pPlayer->setGain(gain);
        }
    }

    virtual const size_t GetSampleRate() const override
    {
        if (m_pPlayer)
        {
            return m_pPlayer->sampleRate();
        }
        return 0;
    }
};

class ymfmMusicRegister final : public musicModuleRegister
{
public:
    ymfmMusicRegister()
    {
        RegisterModule(this);
    }

    virtual bool CheckCompatibleTrack(const void* data, size_t length) override
    {
        // Check if the data is compatible with YMFMIDI format by simply trying to load it. If it fails, return false.
        OPLPlayer* player = new OPLPlayer(1, chipType);
        if(!player->loadPatches(patchData.data(), patchData.size()))
        {
            delete player;
            return false;
        }
        if (!player->loadSequence((const uint8_t*)data, length))
        {
            delete player;
            return false;
        }
        delete player;
        return true;
    }

    virtual musicModule* CreateModuleInstance(const void* data, size_t length) override
    {
        return new ymfmMusic(data, length);
    }

    virtual void RecieveLumpData(const char* name, const void* data, size_t length) override
    {
        if (strncmp(name, "GENMIDI", 7) == 0)
        {
            patchData.assign((const uint8_t*)data, (const uint8_t*)data + length);
        }
    }
};

static ymfmMusicRegister ymfmMusicRegisterInstance;
