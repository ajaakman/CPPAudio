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

#undef main
int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_AUDIO);
	
	IAudioDevice* device = new SDLAudioDevice();

	IAudioData* data = device->CreateAudioFromFile(FILE_PATH);

	SDLAudioContext* context = new SDLAudioContext();
	

	SampleInfo info;
	info.volume = 1.0;

	AudioObject sound(info, data);

	char in = 0;
	while (in != 'q')
	{
		std::cin >> in;
		switch (in)
		{
		case 'a':
			context->PlayAudio(sound);
			break;
		case 's':
			context->PauseAudio(sound);
			break;
		case 'd':
			context->StopAudio(sound);
			break;
		}
	}

	device->ReleaseAudio(data);
	delete context;
	delete device;

	SDL_Quit();
	return 0;
}