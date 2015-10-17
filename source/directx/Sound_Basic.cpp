
#include "Sound_Basic.h"

#include "SoundFileReader.h"
#include "XAudio2SoundPlayer.h"

#include "../../include/Utils.h"

Sound_Basic::Sound_Basic(SoundFileReader* soundFileReader, XAudio2SoundPlayer* soundPlayer)
:m_index(-1), m_soundFileReader(soundFileReader), m_soundPlayer(soundPlayer)
{
}

bool Sound_Basic::isLoaded() const { return m_index != -1; }

void Sound_Basic::play(bool blockUntilEnd)
{
	if (m_soundFileReader->finishedOnce())
	{
		m_index = (int)m_soundPlayer->AddSound(m_soundFileReader->GetSoundFormat(), m_soundFileReader->GetSoundData());
	}
	if (m_index != -1)
	{
		if (blockUntilEnd)
		{
			__debugbreak();//not implemented
		}
		m_soundPlayer->PlaySound(m_index);
	}
}

void Sound_Basic::stop()
{
	if (m_index != -1)
	{
		m_soundPlayer->StopSound(m_index);
	}
}

bool Sound_Basic::isPlaying()
{
	if (m_index != -1)
	{
		return m_soundPlayer->IsSoundPlaying(m_index);
	}
	return false;
}

unsigned char Sound_Basic::getVolume() const
{
	Assert(false);//not implemented
	return 0;
}

void Sound_Basic::setVolume(unsigned char vol)
{
	Assert(false);//not implemented
}

Sound_Basic::~Sound_Basic()
{
}
