
#include "../../include/Utils.h"
#include "Sound_Stream.h"

#include "StreamEffect.h"

void Sound_Stream::manage()
{
	if (m_streamEffect->isAudioStarted())
	{
		m_streamEffect->renderAudio();
	}
}

void Sound_Stream::onSuspending()
{
	m_streamEffect->onSuspending();
}

void Sound_Stream::onResuming()
{
	m_streamEffect->onResuming();
}

Sound_Stream::Sound_Stream(const char* fileName)
{
	m_streamEffect = new StreamEffect(fileName, false);
	m_defaultVolume = m_streamEffect->getVolume();
}

Sound_Stream::~Sound_Stream()
{
	delete m_streamEffect;
}

bool Sound_Stream::isLoaded() const
{
	return true;
}

void Sound_Stream::play(bool blockUntilEnd)
{
	Assert(!blockUntilEnd);// not implemented
	if (!this->isPlaying())
	{
		m_streamEffect->start();
	}
}

void Sound_Stream::stop()
{
	m_streamEffect->stop();
}

bool Sound_Stream::isPlaying()
{
	return m_streamEffect->isAudioStarted();
}

unsigned char Sound_Stream::getVolume() const
{
	return static_cast<unsigned char>(m_streamEffect->getVolume() * 128.f / m_defaultVolume);
}

void Sound_Stream::setVolume(unsigned char vol)
{
	m_streamEffect->setVolume(m_defaultVolume * vol / 128.f);
}
