#ifndef Sound_Basic_h_INCLUDED
#define Sound_Basic_h_INCLUDED

#include "../../include/global_defines.h"
#include "../../include/Sound.h"

class Sound_Basic : public Sound
{
	friend class SoundManager;
public:
	bool isLoaded() const;
	void play(bool blockUntilEnd);
	void stop();
	bool isPlaying();
	unsigned char getVolume() const;
	void setVolume(unsigned char vol);
private:
	~Sound_Basic();
#ifdef USES_JS_EMSCRIPTEN
	Sound_Basic(unsigned wav_length, unsigned char* wav_buffer) :m_wav_length(wav_length), m_wav_buffer(wav_buffer),m_volume(255) {}
	int m_wav_length; // length of the sample
	unsigned char* m_wav_buffer; // buffer containing the audio file
#else
	Sound_Basic(unsigned int parBufferID, unsigned int parSourceID) :bufferID(parBufferID), sourceID(parSourceID),m_volume(255) {}
	unsigned int bufferID;
	unsigned int sourceID;
#endif
	unsigned int m_volume;
	//Sound_Basic():bufferID(0), sourceID(0) {}
	bool isSoundStream() const { return false; }
};

//---------------------------------------------------------------------

#endif //Sound_h_INCLUDED
