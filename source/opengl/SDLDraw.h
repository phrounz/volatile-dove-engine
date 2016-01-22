#ifndef SDLDraw_h_INCLUDED
#define SDLDraw_h_INCLUDED

#include "../../include/global_defines.h"

#include "../../include/CoreUtils.h"

#ifdef USES_SDL_INSTEAD_OF_GLUT

#include "opengl_inc.h"

namespace SDLDraw
{
	void setupSDLRenderer(void* sdlRenderer);

	void drawRectangle(const Int2& pos1, const Int2& pos2, const Color& color);
	void drawLine(const Int2& pos1, const Int2& pos2, const Color& color);
	void drawFragmentOfTexture();
}

#endif

#endif //SDLDraw_h_INCLUDED
