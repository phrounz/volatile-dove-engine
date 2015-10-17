#ifndef Sound_h_INCLUDED
#define Sound_h_INCLUDED

#include "Sound.h"

//---------------------------------------------------------------------
/**
* \brief A playable audio sample
*/

class Sound
{
	friend class SoundManager;
public:
	virtual bool isLoaded() const = 0;
	virtual void play(bool blockUntilEnd = false) = 0;
	virtual void stop() = 0;
	virtual bool isPlaying() = 0;
	virtual unsigned char getVolume() const = 0;
	virtual void setVolume(unsigned char vol) = 0;
protected:
	virtual ~Sound() {};
private:
	virtual bool isSoundStream() const = 0;
};

//---------------------------------------------------------------------

#endif //Sound_h_INCLUDED
