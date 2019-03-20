#include "SDLAudioContext.h"

#include <iostream>

static void SDLAudioContext_AudioCallback(void* userData, Uint8* streamIn, int length)
{
	SDLAudioContext* context = (SDLAudioContext*)userData;
	context->GenerateSamples(streamIn, length);
}

SDLAudioContext::SDLAudioContext()
{
	SDL_AudioSpec spec;
	SDL_zero(spec);
	spec.freq = 44100;
	spec.format = AUDIO_S16SYS;
	spec.channels = 2;
	spec.samples = 2048;

	spec.callback = SDLAudioContext_AudioCallback;
	spec.userdata = this;

	m_Device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);

	if (m_Device == 0)
		std::cout << SDL_GetError() << " | Could not open audio device \n";

	//int success = SDL_QueueAudio(device, wavStart, wavLength);
	SDL_PauseAudioDevice(m_Device, 0);
}

SDLAudioContext::~SDLAudioContext()
{
	SDL_CloseAudioDevice(m_Device);
}

void SDLAudioContext::PlayAudio(AudioObject & ao)
{
	SDL_LockAudioDevice(m_Device);

	RemoveAudio(ao);
	m_PlayingAudio.push_back(&ao);

	SDL_UnlockAudioDevice(m_Device);
}

void SDLAudioContext::PauseAudio(AudioObject & ao)
{
	SDL_LockAudioDevice(m_Device);

	RemoveAudio(ao);

	SDL_UnlockAudioDevice(m_Device);
}

void SDLAudioContext::StopAudio(AudioObject & ao)
{
	SDL_LockAudioDevice(m_Device);

	if (RemoveAudio(ao))
	{
		ao.SetPos(0.0);
	}

	SDL_UnlockAudioDevice(m_Device);
}

void SDLAudioContext::GenerateSamples(Uint8 * streamIn, int streamInLen)
{
	size_t streamLen = (size_t)(streamInLen / 2);

	m_Stream.reserve(streamLen);
	float* floatStream = *(float**)(&m_Stream);

	for (size_t i = 0; i < streamLen; ++i)
	{
		floatStream[i] = 0.0f;
	}

	std::vector<AudioObject*>::iterator it = m_PlayingAudio.begin();
	std::vector<AudioObject*>::iterator end = m_PlayingAudio.end();

	for (; it != end; ++it)
	{
		if (!(*it)->GenerateSamples(floatStream, streamLen))
		{
			RemoveAudio(*(*it));
		}
	}

	Sint16* stream = (Sint16*)streamIn;
	for (size_t i = 0; i < streamLen; ++i)
	{
		float val = floatStream[i];
		if (val > 1.0f)
			val = 1.0f;
		else if (val < -1.0f)
			val = -1.0f;

		stream[i] = (Sint16)(val * 32767);
	}
}

bool SDLAudioContext::RemoveAudio(AudioObject & ao)
{
	std::vector<AudioObject*>::iterator it = m_PlayingAudio.begin();
	std::vector<AudioObject*>::iterator end = m_PlayingAudio.end();

	for (; it != end; ++it)
	{
		if (*it == &ao)
		{
			m_PlayingAudio.erase(it);
			return true;
		}
	}

	return false;
}
