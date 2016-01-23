#ifndef SDLDraw_h_INCLUDED
#define SDLDraw_h_INCLUDED

#include "../../include/global_defines.h"

#include "../../include/CoreUtils.h"

#ifdef USES_SDL_INSTEAD_OF_GLUT

#include "opengl_inc.h"

struct SDL_Surface;
struct SDL_Texture;

namespace SDLDraw
{
	void setupSDLInfos(void* sdlWindow, void* sdlRenderer);
	SDL_Texture* createTextureFromSurface(const SDL_Surface* surface);

	void clearScreen(const Color& color);
	void drawRectangle(const Int2& pos1, const Int2& pos2, const Color& color, bool fill);
	void drawLine(const Int2& pos1, const Int2& pos2, const Color& color);
	void drawTexture(const SDL_Texture* texture, int pixelPosX, int pixelPosY, int width,int height);
	void drawFragmentOfTexture(const SDL_Texture* texture, int pixelPosX,int pixelPosY,float x1,float y1,float x2,float y2, int width,int height, float opacity);
	void drawTextureRotated(const SDL_Texture* texture, 
		int pixelPosX,int pixelPosY, int width,int height, float angleDegree,int rotationCenterX,int rotationCenterY, bool mirrorAxisX,bool mirrorAxisY);
	void resetColor();
}

#endif

#endif //SDLDraw_h_INCLUDED
