#include "AudioObject.h"

AudioObject::AudioObject(const SampleInfo & info, IAudioData * data)
	:m_AudioPos(0)
	,m_AudioLength(data->GetAudioLength())
	,m_SampleInfo(info)
	,m_AudioData(data)
{
}

bool AudioObject::GenerateSamples(float * stream, size_t streamLength)
{
	m_AudioPos = m_AudioData->GenerateSamples(stream, streamLength, m_AudioPos, m_SampleInfo);

	if (m_AudioPos == (size_t)-1)
	{
		m_AudioPos = 0;
		return false;
	}
	
	return true;
}

void AudioObject::SetPos(double pos)
{
	if (pos < 0.0)
		pos = 0.0;
	else if (pos > 1.0)
		pos = 1.0;

	m_AudioPos = PosToAbsolutePos(pos);
}

size_t AudioObject::PosToAbsolutePos(double pos)
{
	return (size_t)(pos * m_AudioLength);
}
