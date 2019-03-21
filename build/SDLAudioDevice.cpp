#include "SDLAudioDevice.h"
#include "SDLWAVAudioData.h"

IAudioData* SDLAudioDevice::CreateAudioFromFile(const std::string& filepath)
{
	return new SDLWAVAudioData(filepath, false);
}

void SDLAudioDevice::ReleaseAudio(IAudioData* audioData)
{
	if (audioData)
		delete audioData;
}