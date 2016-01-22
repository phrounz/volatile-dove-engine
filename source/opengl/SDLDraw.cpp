
#include "opengl_inc.h"
#include "../../include/Utils.h"
#include "../../include/MathUtils.h"

#include "SDLDraw.h"

#ifdef USES_SDL_INSTEAD_OF_GLUT

namespace
{
	SDL_Renderer* s_sdlRenderer = NULL;
}

void SDLDraw::setupSDLRenderer(void* sdlRenderer)
{
	s_sdlRenderer = (SDL_Renderer*)sdlRenderer;
	Assert(s_sdlRenderer != NULL);
}

void SDLDraw::drawRectangle(const Int2& pos1, const Int2& pos2, const Color& color)
{
	SDL_SetRenderDrawColor(s_sdlRenderer, color.r(), color.g(), color.b(), color.a());
	SDL_Rect rect;
	rect.x = MathUtils::minimum(pos1.x(), pos2.x());
	rect.y = MathUtils::minimum(pos1.y(), pos2.y());
	rect.w = abs(pos2.x()-pos1.x());
	rect.h = abs(pos2.x()-pos1.x());
	SDL_RenderDrawRect(s_sdlRenderer, &rect);
	SDL_RenderPresent(s_sdlRenderer);
}

void SDLDraw::drawLine(const Int2& pos1, const Int2& pos2, const Color& color)
{
	SDL_SetRenderDrawColor(s_sdlRenderer, color.r(), color.g(), color.b(), color.a());
	SDL_RenderDrawLine(s_sdlRenderer, pos1.x(), pos1.y(), pos2.x(), pos2.y());
	SDL_RenderPresent(s_sdlRenderer);
}

void SDLDraw::drawFragmentOfTexture()
{
	drawLine(Int2(10,10), Int2(100,100), CoreUtils::colorWhite);
	drawRectangle(Int2(10,10), Int2(40,40), CoreUtils::colorRed);
}

#endif
