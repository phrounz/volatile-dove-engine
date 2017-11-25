
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

void SDLDraw::clearScreen(const Color& color)
{
	SDL_SetRenderDrawColor(s_sdlRenderer, color.r(), color.g(), color.b(), color.a());
	SDL_RenderClear(s_sdlRenderer);
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
	// beware, cast removing constness below, I hate this SDL crap
	SDL_RenderCopy(s_sdlRenderer, (SDL_Texture*)texture, NULL, &destRect);
}


void SDLDraw::drawFragmentOfTexture(const SDL_Texture* texture, int pixelPosX,int pixelPosY,float x1,float y1,float x2,float y2, int width,int height, float opacity)
{
	int w, h;
	// beware, cast removing constness below, I hate this SDL crap
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, &w, &h);

	SDL_Rect srcRect;
	srcRect.x = x1 * w;
	srcRect.y = y1 * h;
	srcRect.w = x2 * w - srcRect.x;
	srcRect.h = y2 * h - srcRect.y;

	SDL_Rect destRect;
	destRect.x = pixelPosX;
	destRect.y = pixelPosY;
	destRect.w = width;
	destRect.h = height;

	// beware, cast removing constness below, I hate this SDL crap
	SDL_RenderCopy(s_sdlRenderer, (SDL_Texture*)texture, &srcRect, &destRect);
}

void SDLDraw::drawTextureRotated(const SDL_Texture* texture,
	int pixelPosX,int pixelPosY, int width,int height, float angleDegree,int rotationCenterX,int rotationCenterY, bool mirrorAxisX,bool mirrorAxisY)
{
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

	#pragma message("TODO SDLDraw::drawTextureRotated")
#if defined(USES_SDL2)
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (mirrorAxisX && mirrorAxisY) flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	else if (mirrorAxisX) flip = SDL_FLIP_HORIZONTAL;
	else if (mirrorAxisY) flip = SDL_FLIP_VERTICAL;
	// cast removing constness below, hate this SDL crap
	SDL_RenderCopyEx(s_sdlRenderer, (SDL_Texture*)texture, NULL, &destRect, angleDegree, NULL, flip);
#else
	SDL_RenderCopy(s_sdlRenderer, (SDL_Texture*)texture, NULL, &destRect);
#endif
}

void SDLDraw::resetColor()
{
}

#endif
