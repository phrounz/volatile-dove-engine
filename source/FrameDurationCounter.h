#ifndef FrameDurationCounter_h_INCLUDED
#define FrameDurationCounter_h_INCLUDED

#include "../include/Utils.h"

//-------------------------------------------------------------------------

namespace
{
	const int MAX_FPS = 60;
}

//-------------------------------------------------------------------------

class FrameDurationCounter
{
public:
	FrameDurationCounter() : m_time(0) {}

	/// prepare for the first frame duration
	void init() { m_time = Utils::getMillisecond(); }

	/// get the last frame duration, prepare for the next
	inline int64_t retrieve()
	{
#if defined(USES_LINUX) || defined(USES_WINDOWS_OPENGL)
		return this->getAndSleepToMaxFPS();
#else
		return this->getNoLimit();
#endif
	}

private:
	
	int64_t getNoLimit()
	{
		int64_t now = Utils::getMillisecond();
		int64_t frameDuration = now - m_time;
		m_time = now;
		return frameDuration;
	}

	// if the framerate (FPS) is greater than MAX_FPS, sleep
	int64_t getAndSleepToMaxFPS() 
	{
		int64_t now = Utils::getMillisecond();
		int64_t frameDuration = now - m_time;
		if (1.0/((double)frameDuration/1000.0) > (double)MAX_FPS) //wait when > MAX_FPS
		{
			Utils::sleepMs(int(1000.0/(double)MAX_FPS - (double)frameDuration));
			frameDuration = (int64_t)(1000.0/(double)MAX_FPS);//10;
			m_time = m_time + frameDuration;
		}
		else
		{
			m_time = now;
		}
		return frameDuration;
	}

	int64_t m_time;
};

//-------------------------------------------------------------------------

#endif //FrameDurationCounter_h_INCLUDED
