
#include "../include/global_defines.h"

#ifdef USES_SDL_FOR_SOUND

	#include "opengl/opengl_inc.h"
	#include "opengl/Sound_Basic.h"

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)

	#ifdef _MSC_VER
		#include <al.h>
		#include <alut.h>
	#else
		#include <AL/al.h>
		#include <AL/alut.h>
	#endif

	#include <cstdio>
	#include <iostream>
	#include <cstring>
	#include <algorithm>

	#include "opengl/Sound_Basic.h"
	#include "opengl/SoundLoad.h"

#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	#include "directx/SoundFileReader.h"
	#include "directx/XAudio2SoundPlayer.h"
	#include "directx/Sound_Basic.h"
	#include "directx/Sound_Stream.h"

#endif

#include "../include/SoundManager.h"
#include "../include/Utils.h"

//-------------------------------------------------------------------------

std::map<std::string, Sound*> SoundManager::m_database;
std::map<Sound*, int> SoundManager::m_counter;

//------------------------------------------------------------------------------

Sound* SoundManager::loadSound(const char* filename, bool alternative)
{
	std::string strFilename = filename;
	std::map<std::string,Sound*>::const_iterator it = m_database.find(strFilename);
	Sound* output = NULL;
	if (it == m_database.end())
	{
		output = loadSound2(filename, alternative);
		m_database[strFilename] = output;
		m_counter[output] = 1;
	}
	else
	{
		output = (*it).second;
		m_counter[output]++;
	}
	return output;
}

//-------------------------------------------------------------------------

Sound* SoundManager::loadSound2(const char* filename, bool alternative)
{
	outputln(std::string("Loading sound:    ") + filename << " (shared)");
#if defined(USES_SDL_FOR_SOUND)
	
	Mix_Chunk* sound = Mix_LoadWAV( filename );
	if (sound == NULL) AssertMessage(false, "Could not open WAV sound file");
	return reinterpret_cast<Sound*>(new Sound_Basic(sound));
#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	unsigned int bufferID;
	unsigned int sourceID;
	SoundLoad::loadSound(filename, &bufferID, &sourceID);
	return reinterpret_cast<Sound*>(new Sound_Basic(bufferID, sourceID));
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	if (alternative)
	{
		Sound_Stream* ss = new Sound_Stream(filename);
		m_soundStreams.push_back(ss);
		return reinterpret_cast<Sound*>(ss);
	}
	else
	{
		return reinterpret_cast<Sound*>(new Sound_Basic(new SoundFileReader(filename), m_soundPlayer));
	}
#else
	#error
#endif
}

//-------------------------------------------------------------------------

void SoundManager::manage()
{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	iterateList(m_soundStreams, Sound_Stream*)
	{
		(*it)->manage();
	}
#endif
}

//-------------------------------------------------------------------------

void SoundManager::onSuspending()
{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	iterateList(m_soundStreams, Sound_Stream*)
	{
		(*it)->onSuspending();
	}
#endif
}

//-------------------------------------------------------------------------

void SoundManager::onResuming()
{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	iterateList(m_soundStreams, Sound_Stream*)
	{
		(*it)->onResuming();
	}
#endif
}

//-------------------------------------------------------------------------

void SoundManager::removeSound(Sound* sound)
{
	m_counter[sound]--;
	if (m_counter[sound] == 0)
	{
		m_counter.erase(sound);
		iterateMapConst(m_database, std::string, Sound*)
		{
			if ((*it).second == sound)
			{
				outputln(std::string("Unloading sound (shared): ") + (*it).first);
				this->removeSound2(sound);
				m_database.erase((*it).first);
				break;
			}
		}
	}
}

//-------------------------------------------------------------------------

void SoundManager::removeSound2(Sound* sound)
{
	if (sound->isSoundStream())
		m_soundStreams.remove(reinterpret_cast<Sound_Stream*>(sound));
	delete sound;
}

//-------------------------------------------------------------------------

SoundManager::SoundManager(const char* argv0)
{
#if defined(USES_SDL_FOR_SOUND)

	 if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1)
	 {
		 Assert(false);
	 }

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)

    // Initialize the OpenAL library
	argvCustom[0] = new char[strlen(argv0)+1];
	strncpy(argvCustom[0], argv0, strlen(argv0)+1);
	argvCustom[1] = NULL;
	int argcCustom = 1;

	ALboolean ret = alutInit(&argcCustom, argvCustom);
	AssertMessage(ret == AL_TRUE, (std::string("SoundManager::SoundManager: ")+alutGetErrorString(alutGetError())).c_str());

#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	const uint32 SOUND_SAMPLE_RATE = 44100;//48000;
	m_soundPlayer = new XAudio2SoundPlayer(SOUND_SAMPLE_RATE);

#else
	#error
#endif
}

//-------------------------------------------------------------------------

SoundManager::~SoundManager()
{
#if defined(USES_SDL_FOR_SOUND)

	iterateMapConst(m_database, std::string, Sound*)
	{
		outputln(std::string("Unloading sound (shared): ") + (*it).first);
		this->removeSound2((*it).second);
		m_database.erase((*it).first);
	}
	Mix_CloseAudio();

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)

	// Clean up the OpenAL library
	delete argvCustom[0];
    ALboolean ret = alutExit();
	AssertMessage(ret == AL_TRUE, (std::string("SoundManager::~SoundManager: ")+alutGetErrorString(alutGetError())).c_str());

#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	delete m_soundPlayer;

#else
	#error
#endif

	Assert(m_soundStreams.size() == 0);// they should have been freed before
}

//-------------------------------------------------------------------------
