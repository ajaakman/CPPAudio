#include "SDLWAVAudioData.h"
#include <iostream>

SDLWAVAudioData::SDLWAVAudioData(const std::string & filename, bool streamFromFile)
{
	//TODO Handle Stream from file.	
	Uint32 Length;
	if (SDL_LoadWAV(filename.c_str(), &m_WavSpec, &m_pStart, &Length) == NULL)
		std::cout << "Error: " << filename << " cound not be loaded as an audio file" << std::endl;

	m_pPos = m_pStart;
	m_pEnd = m_pStart + Length;
}

SDLWAVAudioData::~SDLWAVAudioData()
{
	SDL_FreeWAV(m_pStart);
}

size_t SDLWAVAudioData::GenerateSamples(float* stream, size_t streamLength, size_t pos, const SampleInfo& info)
{
	float pitch = 1.0f;
	m_pPos = m_pStart + pos;

	if (m_pPos >= m_pEnd || m_pPos < m_pStart)
	{
		return (size_t)-1;
	}

	Uint32 length = (Uint32)streamLength;
	Uint32 lengthLeft = (Uint32)((m_pEnd - m_pPos)/pitch);
	length = (length > lengthLeft ? lengthLeft : length);

	// TODO: Make this more general
	Sint16* samples = (Sint16*)m_pPos;
	float sampleIndex = 0;

	float factor = (float)info.volume * 1.0f / 32768.0f;
	for (Uint32 i = 0; i < length; i++)
	{
		stream[i] = samples[(size_t)sampleIndex] * factor;
		sampleIndex += pitch;
	}

	m_pPos = (Uint8*)(samples + (size_t)sampleIndex);

	return (size_t)(m_pPos - m_pStart);
}

size_t SDLWAVAudioData::GetAudioLength()
{
	return (size_t)(m_pEnd - m_pStart);
}