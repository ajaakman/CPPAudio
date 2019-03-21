#pragma once

#include <vector>
#include "AudioObject.h"

#include <SDL.h>

class SDLAudioContext
{
public:
	SDLAudioContext();
	virtual ~SDLAudioContext();
	virtual void PlayAudio(AudioObject& ao);
	virtual void PauseAudio(AudioObject& ao);
	virtual void StopAudio(AudioObject& ao);
	inline SDL_AudioDeviceID GetAudioDevice() { return m_Device; }

	void GenerateSamples(Uint8* stream, int streamLen);
private:
	SDL_AudioDeviceID m_Device;
	std::vector<float> m_Stream;
	std::vector<AudioObject*> m_PlayingAudio;

	bool RemoveAudio(AudioObject& ao);

	SDLAudioContext(SDLAudioContext& other) { (void)other; }
	void operator=(const SDLAudioContext& other) { (void)other; }
};