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
	Sound(const std::string& filepath, SDL_AudioSpec& audiospec, const double & volume)
		:Volume(volume)
	{	
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
	AudioData(const double & volume = 1.0 )
		:MasterVolume(volume)
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

	void Play(const std::string & filepath, double volume = 1.0)
	{
		SDL_LockAudioDevice(m_Device);
		if (volume > 1.0)
			volume = 1.0;
		else if (volume < 0.0)
			volume = 0.0;
		sounds.emplace_front(new Sound(filepath, wavSpec, volume));
		SDL_UnlockAudioDevice(m_Device);
	}	

	double MasterVolume;
	std::forward_list<Sound*> sounds;
	SDL_AudioSpec wavSpec;
	SDL_AudioDeviceID m_Device;
private:
};

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength) // streamLength = samples * channels * bitdepth/8
{
	AudioData* audio = static_cast<AudioData*>(userdata);	

	for (Uint32 i = 0; i < streamLength/2; ++i)
	{
		((Sint16*)stream)[i] = 0;
		for (const auto & sound : audio->sounds)
		{		
			if (sound->RemainingLength/2 > i)
			{	
				const Sint16 addSample = reinterpret_cast<Sint16*>(sound->Position)[i] * sound->Volume;

				if (((Sint16*)stream)[i] < 0)
				{
					if (addSample < -32768 - ((Sint16*)stream)[i])
						((Sint16*)stream)[i] = -32768;
					else
						((Sint16*)stream)[i] += addSample;
				}
				else
				{
					if (addSample > 32767 - ((Sint16*)stream)[i])
						((Sint16*)stream)[i] = 32767;
					else
						((Sint16*)stream)[i] += addSample;
				}
			}			
		}
		((Sint16*)stream)[i] *= audio->MasterVolume;
	}
	
	for (auto & sound : audio->sounds)
	{
		Uint32 length = ((Uint32)streamLength > sound->RemainingLength ? sound->RemainingLength : (Uint32)streamLength);
		sound->Position += length;
		sound->RemainingLength -= length;
	}

	audio->sounds.remove_if([](Sound*& value) { if (value->RemainingLength) return false; else { delete value; return true; } } );
}

#undef main
int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_AUDIO);

	AudioData audio(1.0);	
	
	audio.Play("./res/audio/TestFile16bit.wav", 1.0);

	//SDL_Delay(1000);	

	//audio.Play("./res/audio/TestFile16bit.wav", 0.3);

	//SDL_Delay(1000);

	//audio.Play("./res/audio/TestFile16bit.wav", 0.3);

	//SDL_Delay(1000);

	while (1);
	
	SDL_Quit();
	return 0;
}