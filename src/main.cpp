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


class AudioData
{
public:
	std::vector<Uint8*> pos;
	std::vector<Uint32> length;
};

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)// streamLength = samples * channels * numofbits/8
{
	AudioData* audio = (AudioData*)userdata;
			
	
	for (Uint32 i = 0; i < streamLength/2; i++)
	{
		((Sint16*)stream)[i] = 0;

		for (int j = 0; j < audio->pos.size(); ++j)
		{
			if ((audio->length)[j] > streamLength / 2 - i)
				((Sint16*)stream)[i] += (((Sint16*)((audio->pos)[j]))[i] * 0.5);
			else
				;//std::cout << "cut end" << std::endl;
		}



		((Sint16*)stream)[i] *= 0.1; // Master Volume
	}
	//SDL_memcpy(stream, audio->pos, length);

	for (int j = 0; j < audio->pos.size(); ++j)
	{
		Uint32 length = (Uint32)streamLength;
		length = (length > (audio->length)[j] ? (audio->length)[j] : length);
		(audio->pos)[j] += length;
		(audio->length)[j] -= length;
	}
}

#undef main
int main(int argc, char** argv)
{
	// Just to make sure SDL is working
	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec wavSpec;
	SDL_memset(&wavSpec, 0, sizeof(wavSpec));

	AudioData audio;

	wavSpec.userdata = &audio;
	wavSpec.channels = 1;
	wavSpec.freq = 44100;
	wavSpec.format = AUDIO_S16SYS;
	wavSpec.samples = 2048;
	wavSpec.callback = MyAudioCallback;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	SDL_PauseAudioDevice(device, 0);

	if (device == 0)
		std::cout << SDL_GetError() << " | Could not open audio device \n";



	Uint8* wavBuffer;
	Uint32 wavLength;

	if (SDL_LoadWAV("./res/audio/TestFile16bit.wav", &wavSpec, &wavBuffer, &wavLength) == NULL)
		std::cout << "Error: " << "./res/audio/testClip.wav" << " cound not be loaded as an audio file" << std::endl;	

	audio.pos.push_back(wavBuffer);
	audio.length.push_back(wavLength);

	SDL_Delay(1000);

	Uint8* wavBuffer1;
	Uint32 wavLength1;

	if (SDL_LoadWAV("./res/audio/TestFile16bit.wav", &wavSpec, &wavBuffer1, &wavLength1) == NULL)
		std::cout << "Error: " << "./res/audio/testClip.wav" << " cound not be loaded as an audio file" << std::endl;

	audio.pos.push_back(wavBuffer1);
	audio.length.push_back(wavLength1);


	//int success = SDL_QueueAudio(device, wavBuffer, wavLength);

	while (1);

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wavBuffer);
	SDL_Quit();
	return 0;
}