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

#define FILE_PATH "./res/audio/testClip.wav"

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


struct AudioData
{
	Uint8* pos;
	Uint32 length;
};

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength)
{
	AudioData* audio = (AudioData*)userdata;

	if (audio->length == 0)
	{
		return;
	}

	for (Uint32 i = 0; i < streamLength/2 ; i++)
	{
		((Sint16*)audio->pos)[i] *= 0.1;
	}
	//std::cout << streamLength << std::endl; // sample * 4


	Uint32 length = (Uint32)streamLength;
	length = (length > audio->length ? audio->length : length);


	SDL_memcpy(stream, audio->pos, length);

	audio->pos += length;
	audio->length -= length;
}

#undef main
int main(int argc, char** argv)
{
	// Just to make sure SDL is working
	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec wavSpec;
	Uint8* wavBuffer;
	Uint32 wavLength;

	if (SDL_LoadWAV(FILE_PATH, &wavSpec, &wavBuffer, &wavLength) == NULL)
		std::cout << "Error: " << FILE_PATH << " cound not be loaded as an audio file" << std::endl;	

	AudioData audio;
	audio.pos = wavBuffer;
	audio.length = wavLength;

	//SDL_memset(&wavSpec, 0, sizeof(wavSpec));
	wavSpec.userdata = &audio;
	wavSpec.channels = 2;
	wavSpec.freq = 44100;
	wavSpec.format = AUDIO_S16SYS;
	wavSpec.samples = 2048;
	wavSpec.callback = MyAudioCallback;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

	if (device == 0)
		std::cout << SDL_GetError() << " | Could not open audio device \n";

	//int success = SDL_QueueAudio(device, wavStart, wavLength);
	SDL_PauseAudioDevice(device, 0);

	while (audio.length > 0)
		SDL_Delay(100);

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wavBuffer);
	SDL_Quit();
	return 0;
}