#pragma once

#include "IAudioContext.h"

#include <vector>

#include <SDL.h>

class SDLAudioContext : public IAudioContext
{
public:
	SDLAudioContext();
	virtual ~SDLAudioContext();
	virtual void PlayAudio(AudioObject& ao);
	virtual void PauseAudio(AudioObject& ao);
	virtual void StopAudio(AudioObject& ao);

	void GenerateSamples(Uint8* stream, int streamLen);
private:
	SDL_AudioDeviceID m_Device;
	std::vector<float> m_Stream;
	std::vector<AudioObject*> m_PlayingAudio;

	bool RemoveAudio(AudioObject& ao);

	SDLAudioContext(SDLAudioContext& other) { (void)other; }
	void operator=(const SDLAudioContext& other) { (void)other; }
};