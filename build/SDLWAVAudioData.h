#pragma once

#include "IAudioData.h"
#include "SDL.h"
#include <string>

class SDLWAVAudioData : public IAudioData
{
public:
	SDLWAVAudioData(const std::string& filename, bool streamFromFile);
	virtual ~SDLWAVAudioData();
	virtual size_t GenerateSamples(float* stream, size_t streamLength, size_t pos, const SampleInfo& info);
	virtual size_t GetAudioLength();
private:
	Uint8* m_pPos;
	Uint8* m_pStart;
	Uint32 m_nLength;
	SDL_AudioSpec m_WavSpec;

	SDLWAVAudioData(SDLWAVAudioData& other) { (void)other; }
	void operator=(const SDLWAVAudioData* other) { (void)other; }
};