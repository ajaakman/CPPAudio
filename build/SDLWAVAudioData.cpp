#include "SDLWAVAudioData.h"
#include <iostream>

SDLWAVAudioData::SDLWAVAudioData(const std::string & filename, bool streamFromFile)
{
	//TODO Handle Stream from file.	

	if (SDL_LoadWAV(filename.c_str(), &m_WavSpec, &m_pStart, &m_nLength) == NULL)
		std::cout << "Error: " << filename << " cound not be loaded as an audio file" << std::endl;

	m_pPos = m_pStart;
}

SDLWAVAudioData::~SDLWAVAudioData()
{
	SDL_FreeWAV(m_pStart);
}

size_t SDLWAVAudioData::GenerateSamples(float* stream, size_t streamLength, size_t pos, const SampleInfo& info)
{
	// TODO Handle pos.
	if (m_nLength == 0)
	{
		return (size_t)-1;
	}

	Uint32 length = (Uint32)streamLength;
	length = (length > m_nLength ? m_nLength : length);

	// TODO: Make this more general
	Sint16* samples = (Sint16*)m_pPos;

	float factor = (float)info.volume * 1.0f / 32768.0f;
	for (Uint32 i = 0; i < length; i++)
	{
		stream[i] = (*samples) * factor;
		samples++;
	}

	m_pPos = (Uint8*)samples;
	m_nLength -= (length * 2);

	return length;
}

size_t SDLWAVAudioData::GetAudioLength()
{
	return m_nLength;
}