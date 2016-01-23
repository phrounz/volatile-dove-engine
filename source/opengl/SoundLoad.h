#ifndef SoundLoad_h_INCLUDED
#define SoundLoad_h_INCLUDED

#if !defined(USES_JS_EMSCRIPTEN)

namespace SoundLoad
{
	void loadSound(const char* fileName, unsigned int* outBufferId, unsigned int* outSourceId);
}

#endif //!defined(USES_JS_EMSCRIPTEN)

#endif //SoundLoad_h_INCLUDED
