#include "player.h"
#include "ymfmidiCPlayer.h"

static OPLPlayer* m_pPlayer = nullptr;

// ---------------------------------------------------------------------------------
//
void YMFMIDI_Init(int numChips, int chipType)
{
    if (m_pPlayer)
    {
        delete m_pPlayer;
		m_pPlayer = nullptr;
    }

    OPLPlayer::ChipType ymfmChipType;
    if(chipType == 1)
    {
        ymfmChipType = OPLPlayer::ChipType::ChipOPL;
    }
    else if(chipType == 2)
    {
        ymfmChipType = OPLPlayer::ChipType::ChipOPL2;
    }
    else
    {
        ymfmChipType = OPLPlayer::ChipType::ChipOPL3;
    }

    m_pPlayer = new OPLPlayer(numChips, ymfmChipType);
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_Shutdown()
{
    if(m_pPlayer != nullptr)
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
    }
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_SetLoop(int loop)
{
    if (m_pPlayer)
    {
        m_pPlayer->setLoop(loop);
    }
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_SetStereo(int on)
{
    if (m_pPlayer)
    {
        m_pPlayer->setStereo(on);
    }
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_SetSampleRate(unsigned int rate)
{
    if (m_pPlayer)
    {
        m_pPlayer->setSampleRate(rate);
    }
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_SetGain(double gain)
{
    if (m_pPlayer)
    {
        m_pPlayer->setGain(gain);
    }
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_SetStereo(bool on)
{
    if (m_pPlayer)
    {
        m_pPlayer->setStereo(on);
    }
}

// ---------------------------------------------------------------------------------
//
int YMFMIDI_LoadSequence(const unsigned char* data, unsigned int size)
{
    if (m_pPlayer)
    {
		return m_pPlayer->loadSequence(data, size);
    }
    return 0;
}

// ---------------------------------------------------------------------------------
//
int YMFMIDI_LoadPatches(const unsigned char* data, unsigned int size)
{
    if (m_pPlayer)
    {
        return m_pPlayer->loadPatches(data, size);
    }
    return 0;
}

// ---------------------------------------------------------------------------------
//
void YMFMIDI_Reset()
{
    if (m_pPlayer)
    {
        m_pPlayer->reset();
    }
}

// ---------------------------------------------------------------------------------
//
int YMFMIDI_AtEnd()
{
    if (m_pPlayer)
    {
        return m_pPlayer->atEnd();
    }
    return 1;
}

// ---------------------------------------------------------------------------------
//
uint32_t YMFMIDI_SampleRate()
{
    if (m_pPlayer)
    {
        return m_pPlayer->sampleRate();
    }
    return 0;
}

// ---------------------------------------------------------------------------------
void YMFMIDI_Generatef(float* data, unsigned int numSamples)
{
    if (m_pPlayer)
    {
        m_pPlayer->generate(data, numSamples);
    }
}

// ---------------------------------------------------------------------------------
void YMFMIDI_Generate16(signed short* data, unsigned int numSamples)
{
    if (m_pPlayer)
    {
        m_pPlayer->generate(data, numSamples);
    }
}
