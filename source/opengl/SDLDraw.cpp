
#include "opengl_inc.h"
#include "../../include/Utils.h"
#include "../../include/MathUtils.h"

#include "SDLDraw.h"

#ifdef USES_SDL_INSTEAD_OF_GLUT

namespace
{
	SDL_Window* s_sdlWindow = NULL;
	SDL_Renderer* s_sdlRenderer = NULL;
}

void SDLDraw::setupSDLInfos(void* sdlWindow, void* sdlRenderer)
{
	s_sdlRenderer = (SDL_Renderer*)sdlRenderer;
	Assert(s_sdlRenderer != NULL);
}

SDL_Texture* SDLDraw::createTextureFromSurface(const SDL_Surface* surface)
{
	// cast removing constness below, hate this SDL crap
	return SDL_CreateTextureFromSurface(s_sdlRenderer, (SDL_Surface*)surface);
}

void SDLDraw::drawRectangle(const Int2& pos1, const Int2& pos2, const Color& color, bool fill)
{
	SDL_SetRenderDrawColor(s_sdlRenderer, color.r(), color.g(), color.b(), color.a());
	SDL_Rect rect;
	rect.x = MathUtils::minimum(pos1.x(), pos2.x());
	rect.y = MathUtils::minimum(pos1.y(), pos2.y());
	rect.w = abs(pos2.x()-pos1.x());
	rect.h = abs(pos2.x()-pos1.x());
	if (fill) SDL_RenderFillRect(s_sdlRenderer, &rect);
	else SDL_RenderDrawRect(s_sdlRenderer, &rect);
}

void SDLDraw::drawLine(const Int2& pos1, const Int2& pos2, const Color& color)
{
	SDL_SetRenderDrawColor(s_sdlRenderer, color.r(), color.g(), color.b(), color.a());
	SDL_RenderDrawLine(s_sdlRenderer, pos1.x(), pos1.y(), pos2.x(), pos2.y());
}

void SDLDraw::drawTexture(const SDL_Texture* texture, int pixelPosX, int pixelPosY, int width,int height)
{
	SDL_Rect destRect;
	destRect.x = pixelPosX;
	destRect.y = pixelPosY;
	destRect.w = width;
	destRect.h = height;
	//SDL_BlitSurface((SDL_Surface*)image, NULL, SDL_GetWindowSurface(s_sdlWindow), &destRect);
	//drawLine(Int2(10,10), Int2(100,100), CoreUtils::colorWhite);
	// cast removing constness below, hate this SDL crap
	SDL_RenderCopy(s_sdlRenderer, (SDL_Texture*)texture, NULL, &destRect);

}


void SDLDraw::drawFragmentOfTexture(const SDL_Texture* texture, int pixelPosX,int pixelPosY,float x1,float y1,float x2,float y2, int width,int height, float opacity)
{
	SDL_Rect srcRect;
	srcRect.x = x1;
	srcRect.y = y2;
	srcRect.w = width;
	srcRect.h = height;

	SDL_Rect destRect;
	destRect.x = pixelPosX;
	destRect.y = pixelPosY;
	destRect.w = width;
	destRect.h = height;

	#pragma message("TODO SDLDraw::drawFragmentOfTexture srcRect")

	// cast removing constness below, hate this SDL crap
	//SDL_BlitSurface((SDL_Surface*)image, NULL, SDL_GetWindowSurface(s_sdlWindow), &destRect);
}

void SDLDraw::drawTextureRotated(const SDL_Texture* texture, 
	int pixelPosX,int pixelPosY, int width,int height, float angleDegree,int rotationCenterX,int rotationCenterY, bool mirrorAxisX,bool mirrorAxisY)
{
	#pragma message("TODO SDLDraw::drawTextureRotated")
	/*SDL_Rect srcRect;
	srcRect.x = x1;
	srcRect.y = y2;
	srcRect.w = width;
	srcRect.h = height;*/

	SDL_Rect destRect;
	destRect.x = pixelPosX;
	destRect.y = pixelPosY;
	destRect.w = width;
	destRect.h = height;

	// cast removing constness below, hate this SDL crap
	//SDL_BlitSurface((SDL_Surface*)image, NULL, SDL_GetWindowSurface(s_sdlWindow), &destRect);
}

void SDLDraw::resetColor()
{
}

#endif
