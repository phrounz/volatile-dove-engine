#ifndef Sound_Basic_h_INCLUDED
#define Sound_Basic_h_INCLUDED

#include "../../include/Sound.h"

class XAudio2SoundPlayer;
class SoundFileReader;

//---------------------------------------------------------------------

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
	int m_index;
	XAudio2SoundPlayer* m_soundPlayer;
	SoundFileReader* m_soundFileReader;
	Sound_Basic(SoundFileReader* soundFileReader, XAudio2SoundPlayer* soundPlayer);
	bool isSoundStream() const { return false; }
};

//---------------------------------------------------------------------

#endif //Sound_h_INCLUDED
