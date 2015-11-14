#ifndef SoundManager_h_INCLUDED
#define SoundManager_h_INCLUDED

#include <map>
#include <list>

#include "Sound.h"

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
class XAudio2SoundPlayer;
#endif

class Sound_Stream;

//---------------------------------------------------------------------
/**
* \brief The audio samples manager (singleton).
*
* Note: use a database of commonly shared Sounds so not totally thread-safe.
*/

class SoundManager
{
	friend class Engine;
public:

	/// load a sound. 
	/// - On windows/opengl: .ogg and .wav are are supported
	/// - On windows 8 metro: only .wav pcm/adpcm and wma (with alternative = true) are supported
	Sound* loadSound(const char* fileName, bool alternative = true);

	void manage();
	void onSuspending();
	void onResuming();

	void removeSound(Sound* sound);

private:
	Sound* loadSound2(const char* fileName, bool alternative);
	void removeSound2(Sound* sound);

	SoundManager(const char* argv0);
	~SoundManager();
	char* argvCustom[2];
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	XAudio2SoundPlayer* m_soundPlayer;
#endif
	std::list<Sound_Stream*> m_soundStreams;

	static std::map<std::string, Sound*> m_database;
	static std::map<Sound*, int> m_counter;

};

//---------------------------------------------------------------------

#endif //SoundManager_h_INCLUDED
