
#include "../../include/global_defines.h"

#ifdef USES_SDL_FOR_SOUND
	#include "opengl_inc.h"
#elif defined(USES_WINDOWS_OPENGL)
	#include <al.h>
	#include <alut.h>
#else
	#include <AL/al.h>
	#include <AL/alut.h>
#endif

#include "../../include/Utils.h"

#include "Sound_Basic.h"

//------------------------------------------------------------------------------
bool Sound_Basic::isLoaded() const { return true;}
//------------------------------------------------------------------------------
void Sound_Basic::play(bool blockUntilEnd)
{
#ifdef USES_SDL_FOR_SOUND
	#pragma message("TODO Sound_Basic::isPlaying")
	SDL_PauseAudio(0);
#else
    // Finally, play the sound!!!
    alSourcePlay(sourceID);

    if (blockUntilEnd)
    {
        ALint state;// The state of the sound source

        do {
            // Query the state of the souce
            alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
            Utils::sleepMs(10);
        } while (state != AL_STOPPED);
    }
#endif
}
//------------------------------------------------------------------------------
void Sound_Basic::stop()
{
#ifdef USES_SDL_FOR_SOUND
	#pragma message("TODO Sound_Basic::isPlaying")
#else
    alSourceStop(sourceID);
#endif
}
//------------------------------------------------------------------------------
bool Sound_Basic::isPlaying()
{
#ifdef USES_SDL_FOR_SOUND
	#pragma message("TODO Sound_Basic::isPlaying")
	return false;
#else
    ALint state;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    return state != AL_STOPPED && state != AL_INITIAL;
#endif
}
//------------------------------------------------------------------------------
void Sound_Basic::setVolume(unsigned char vol)
{
#ifdef USES_SDL_FOR_SOUND
	#pragma message("TODO Sound_Basic::setVolume")
#else
	m_volume = vol;
	alSourcef(sourceID, AL_GAIN, vol / 256.f);
    //alSource3f(sourceID, AL_POSITION, 2000000+255.f*100 - (float)vol*100, 0.0f, 0.0f);
#endif
}
//------------------------------------------------------------------------------
unsigned char Sound_Basic::getVolume() const
{
	return m_volume;
}
//------------------------------------------------------------------------------
Sound_Basic::~Sound_Basic()
{
#ifdef USES_SDL_FOR_SOUND
	SDL_FreeWAV(m_wav_buffer);
#else
	// http://gamedev.stackexchange.com/questions/25671/how-should-i-unbind-and-delete-openal-buffers
	ALint sourceState;
	alGetSourcei(sourceID, AL_SOURCE_STATE, &sourceState);
	if (sourceState == AL_PLAYING) alSourceStop(sourceID);
	alSourcei(sourceID, AL_BUFFER, 0);//NULL);

    // Clean up sound buffer
    alDeleteBuffers(1, &bufferID);
    // Clean up sound source
    alDeleteSources(1, &sourceID);
#endif
}
//------------------------------------------------------------------------------
