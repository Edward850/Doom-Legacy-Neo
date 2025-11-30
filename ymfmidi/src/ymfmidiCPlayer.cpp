#include "player.h"
#include "ymfmidiCPlayer.h"
#include <thread>
#include <atomic>

static OPLPlayer* m_pPlayer = nullptr;
static std::thread* m_thread;
static std::vector<std::vector<signed short>> m_threadSamples;
static int m_threadSampleIndex = 0;
static std::atomic<int> m_sampleCountNeeded;
static std::atomic<int> m_sampleCountReady;
static std::atomic<bool> m_threadTerminate;

// ---------------------------------------------------------------------------------
//
void YMFMIDI_Init(int numChips, int chipType)
{
    if (m_pPlayer)
    {
        m_threadTerminate = true;
        m_thread->join();
        m_threadTerminate = false;
        delete m_thread;
        m_thread = nullptr;
        delete m_pPlayer;
		m_pPlayer = nullptr;
    }

	m_threadSamples.clear();
    m_sampleCountNeeded = 0;
	m_sampleCountReady = 0;
    m_threadSampleIndex = 0;

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
        m_threadTerminate = true;
        m_thread->join();
        m_threadTerminate = false;
        delete m_thread;
        m_thread = nullptr;
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
        m_threadTerminate = true;
        m_thread->join();
        m_threadTerminate = false;
        delete m_thread;
        m_thread = nullptr;
        m_pPlayer->reset();

		m_sampleCountNeeded = 0;
        m_sampleCountReady = 0;
		m_threadSampleIndex = 0;
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
int YMFMIDI_SamplesReady()
{
	return m_sampleCountReady.load();
}

// ---------------------------------------------------------------------------------
void YMFMIDI_SamplesCountAdd()
{
    if (m_sampleCountNeeded.load() < 10)
    {
        m_sampleCountNeeded++;
    }
}

// ---------------------------------------------------------------------------------
static void YMFMIDI_Generate16Thread(unsigned int numSamples)
{
	m_threadSamples = std::vector<std::vector<signed short>>(10, std::vector<signed short>(numSamples * 2));
	int threadSampleIndex = 0;
	while (m_threadTerminate.load() == false)
    {
        if (m_sampleCountNeeded.load())
        {
            m_pPlayer->generate(m_threadSamples[threadSampleIndex].data(), numSamples);
            m_sampleCountNeeded--;
            m_sampleCountReady++;
            threadSampleIndex++;
            if (threadSampleIndex >= m_threadSamples.size())
            {
                threadSampleIndex = 0;
            }
        }
        else
        {
            std::this_thread::yield();
        }
	}
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
    if (m_sampleCountReady.load())
    {
		memcpy(data, m_threadSamples[m_threadSampleIndex].data(), m_threadSamples[m_threadSampleIndex].size() * 2);
        m_sampleCountReady--;
        m_threadSampleIndex++;
		if (m_threadSampleIndex >= m_threadSamples.size())
        {
            m_threadSampleIndex = 0;
        }
    }

    if (m_pPlayer)
    {
        if(!m_thread)
        {
            m_sampleCountNeeded = 10;
			m_thread = new std::thread(YMFMIDI_Generate16Thread, numSamples);
		}
    }
}
