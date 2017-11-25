#ifndef opengl_inc_h_INCLUDED
#define opengl_inc_h_INCLUDED

#include "../../include/global_defines.h"

#if defined(USES_JS_EMSCRIPTEN) //__EMSCRIPTEN__
	#include <emscripten.h>
	#define GL_GLEXT_PROTOTYPES

	#ifdef USES_SDL_INSTEAD_OF_GLUT
		// https://stackoverflow.com/questions/37568585/unknown-type-name-sdl-rendererflip-using-sdl2-emscripten
		#ifdef USES_SDL2
			#include <SDL.h>
		#else
			#include <SDL/SDL.h>
			#include <SDL/SDL_image.h>
			#include <SDL/SDL_opengl.h>
		#endif
		#include <GL/gl.h>
		#include <GL/glu.h>
	#else
		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glut.h>
		#include <GL/freeglut_std.h>
	#endif

	//#include <stdio.h>
	//#include <EGL/egl.h>
	//#include <EGL/eglext.h>

#elif defined(USES_WINDOWS_OPENGL)

	#include <windows.h>
	#include <WinGDI.h>

	#ifdef USES_SDL_INSTEAD_OF_GLUT
		#include "SDL/SDL.h"
		//#include "SDL/SDL_image.h"
		#include "SDL/SDL_opengl.h"
		#include <gl/GL.h>
		#include <gl/GLU.h>
		#include <GL/glut.h>
		#include <GL/freeglut_ext.h>
		#include <GL/glext.h>
	#else
		#include <gl/GL.h>
		#include <gl/GLU.h>
		#include <GL/glut.h>
		#include <GL/freeglut_ext.h>
		#include <GL/glext.h>
	#endif

#elif defined (USES_LINUX)

	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>

	#define USES_SDL_FOR_SOUND

#else
	#error
#endif

#ifndef USES_SDL_INSTEAD_OF_GLUT
	#if !defined(GLUT_WHEEL_UP)
	#  define GLUT_WHEEL_UP   3
	#  define GLUT_WHEEL_DOWN 4
	#endif
#endif

#ifdef USES_SDL_FOR_SOUND
	#include "SDL/SDL_mixer.h"
	#include "SDL/SDL_audio.h"
#endif

#endif //opengl_inc_h_INCLUDED
