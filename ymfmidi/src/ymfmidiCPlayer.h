#ifndef YMFMIDICPLAYER_H
#define YMFMIDICPLAYER_H

#ifdef __cplusplus
extern "C" {
#endif
	void YMFMIDI_Init(int numChips, int chipType);
	void YMFMIDI_Shutdown(void);
	void YMFMIDI_SetLoop(int loop);
	void YMFMIDI_SetStereo(int on);
	void YMFMIDI_SetSampleRate(unsigned int rate);
	void YMFMIDI_SetGain(double gain);
	int YMFMIDI_LoadSequence(const unsigned char* data, unsigned int size);
	int YMFMIDI_LoadPatches(const unsigned char* data, unsigned int size);
	void YMFMIDI_Reset(void);
	int YMFMIDI_AtEnd(void);
	unsigned int YMFMIDI_SampleRate(void);
	int YMFMIDI_SamplesReady(void);
	void YMFMIDI_SamplesCountAdd(void);
	void YMFMIDI_Generatef(float* data, unsigned int numSamples);
	void YMFMIDI_Generate16(signed short* data, unsigned int numSamples);
#ifdef __cplusplus
}
#endif
#endif