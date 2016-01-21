#ifndef opengl_inc_h_INCLUDED
#define opengl_inc_h_INCLUDED

#if defined(USES_WINDOWS_OPENGL)
	#include <windows.h>
	#include <WinGDI.h>
	#include <gl/GL.h>
	#include <gl/GLU.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
	#include <GL/glext.h>
#elif defined (USES_LINUX) && !defined(USES_JS_EMSCRIPTEN)
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
#elif defined (USES_LINUX) && defined(USES_JS_EMSCRIPTEN)
	#include <emscripten.h>
	#define GL_GLEXT_PROTOTYPES

	#include "SDL/SDL.h"
	#include "SDL/SDL_image.h"
	#include "SDL/SDL_opengl.h"

/*	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut_std.h>*/

	//#include <stdio.h>
	//#include <EGL/egl.h>
	//#include <EGL/eglext.h>

	//#ifdef __EMSCRIPTEN__
	//#include <emscripten.h>
	//#endif
#else
	#error
#endif

#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

#endif //opengl_inc_h_INCLUDED
