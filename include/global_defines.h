#ifndef global_defines_h_INCLUDED
#define global_defines_h_INCLUDED

#ifdef USES_JS_EMSCRIPTEN
	#define USES_SDL_INSTEAD_OF_GLUT
#else
	#define USES_SOUND
	#define USES_SCENE3D
#endif

#endif //global_defines_h_INCLUDED
