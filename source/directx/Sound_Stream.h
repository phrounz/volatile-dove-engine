#ifndef Sound_Stream_h_INCLUDED
#define Sound_Stream_h_INCLUDED

#include "../../include/Sound.h"

class StreamEffect;

//---------------------------------------------------------------------

class Sound_Stream : public Sound
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
	~Sound_Stream();
	void manage();
	void onSuspending();
	void onResuming();
	Sound_Stream(const char* fileName);
	mutable StreamEffect* m_streamEffect;
	bool isSoundStream() const { return true; }
	float m_defaultVolume;
};

//---------------------------------------------------------------------

#endif //Sound_Stream_h_INCLUDED
