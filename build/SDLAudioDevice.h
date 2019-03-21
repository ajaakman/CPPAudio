#pragma once

#include "IAudioDevice.h"

class SDLAudioDevice: public IAudioDevice
{
public:
	virtual IAudioData* CreateAudioFromFile(const std::string& filepath);
	virtual void ReleaseAudio(IAudioData* audioData);
private:
};