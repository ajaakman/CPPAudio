/*
 * Copyright (c) 2014, Benny Bobaganoosh
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED ANDON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <SDL.h>
#include <forward_list>

#include "../build/IAudioDevice.h"
#include "../build/IAudioContext.h"
#include "../build/AudioObject.h"

#include "../build/SDLAudioDevice.h"
#include "../build/SDLAudioContext.h"


//#undef main
//int main(int argc, char** argv)
//{
//	SDL_Init(SDL_INIT_AUDIO);
//	
//	IAudioDevice* device = new SDLAudioDevice();
//
//	IAudioData* data = device->CreateAudioFromFile(FILE_PATH);
//
//	SDLAudioContext* context = new SDLAudioContext();
//	
//
//	SampleInfo info;
//	info.volume = 1.0;
//
//	AudioObject sound(info, data);
//
//	char in = 0;
//	while (in != 'q')
//	{
//		std::cin >> in;
//		switch (in)
//		{
//		case 'a':
//			context->PlayAudio(sound);
//			break;
//		case 's':
//			context->PauseAudio(sound);
//			break;
//		case 'd':
//			context->StopAudio(sound);
//			break;
//		}
//	}
//
//	device->ReleaseAudio(data);
//	delete context;
//	delete device;
//
//	SDL_Quit();
//	return 0;
//}
class Sound
{
public:
	Sound(const std::string& filepath, SDL_AudioSpec& audiospec)
		:Volume(1.0)
	{	
		std::cout << "construcer\n";
		if (SDL_LoadWAV("./res/audio/TestFile16bit.wav", &audiospec, &Buffer, &TotalLength) == NULL)
			std::cout << "Error: " << filepath << " cound not be loaded as an audio file" << std::endl;
		Position = Buffer;
		RemainingLength = TotalLength;
	}
	~Sound()
	{
		std::cout << "sound destroyed" << std::endl;
		SDL_FreeWAV(Buffer);
	}
	Uint8* Buffer;
	Uint8* Position;
	Uint32 TotalLength;
	Uint32 RemainingLength;
	double Volume;
};

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength);

struct AudioData
{
	AudioData()
		:MasterVolume(1.0)
	{
		SDL_memset(&wavSpec, 0, sizeof(wavSpec));
		
		wavSpec.userdata = this;
		wavSpec.channels = 1;
		wavSpec.freq = 44100;
		wavSpec.format = AUDIO_S16SYS;
		wavSpec.samples = 2048;
		wavSpec.callback = MyAudioCallback;

		m_Device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

		if (m_Device == 0)
			std::cout << SDL_GetError() << " | Could not open audio device \n";

		SDL_PauseAudioDevice(m_Device, 0);
	}
	~AudioData()
	{
		SDL_CloseAudioDevice(m_Device);
	}

	void Play(const Sound& sound)
	{
		SDL_LockAudioDevice(m_Device);
		sounds.emplace_front(sound);
		SDL_UnlockAudioDevice(m_Device);
	}

	Sint16 CurrentSample;
	double MasterVolume;
	std::forward_list<Sound> sounds;
	SDL_AudioSpec wavSpec;
private:
	SDL_AudioDeviceID m_Device;
};

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength) // streamLength = samples * channels * bitdepth/8
{
	AudioData* audio = static_cast<AudioData*>(userdata);
	
	for (Uint32 i = 0; i < streamLength/2; ++i)
	{
		audio->CurrentSample = 0;
		for (const auto & sound : audio->sounds)
		{
			if (sound.RemainingLength > streamLength / 2 - i)
			{
				Sint16 addSample = reinterpret_cast<Sint16*>(sound.Position)[i] * sound.Volume;
				
				if (audio->CurrentSample < 0)
				{
					if (addSample < -32768 - audio->CurrentSample)
						audio->CurrentSample = -32768;
					else
						audio->CurrentSample += addSample;
				}
				else
				{
					if (addSample > 32767 - audio->CurrentSample)
						audio->CurrentSample = 32767;
					else
						audio->CurrentSample += addSample;
				}
			}
			//else std::cout << "cut end" << std::endl;
		}

		((Sint16*)stream)[i] = audio->CurrentSample * audio->MasterVolume;
	}

	for (auto & sound : audio->sounds)
	{
		Uint32 length = (Uint32)streamLength;
		length = (length > sound.RemainingLength ? sound.RemainingLength : length);
		sound.Position += length;
		sound.RemainingLength -= length;
	}
}

#undef main
int main(int argc, char** argv)
{
	// Just to make sure SDL is working
	SDL_Init(SDL_INIT_AUDIO);

	AudioData audio;	
	
	Sound s1("./res/audio/TestFile16bit.wav", audio.wavSpec);
	audio.Play(s1);

	SDL_Delay(1000);

	Sound s2("./res/audio/TestFile16bit.wav", audio.wavSpec);
	audio.Play(s2);

	SDL_Delay(1000);

	Sound s3("./res/audio/TestFile16bit.wav", audio.wavSpec);
	audio.Play(s3);

	SDL_Delay(1000);

	Sound s4("./res/audio/TestFile16bit.wav", audio.wavSpec);
	audio.Play(s4);

	while (1);
	
	SDL_Quit();
	return 0;
}