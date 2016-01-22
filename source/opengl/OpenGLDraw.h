#ifndef OpenGLDraw_h_INCLUDED
#define OpenGLDraw_h_INCLUDED

#include "../../include/global_defines.h"

#ifndef USES_SDL_INSTEAD_OF_GLUT

#include "../../include/CoreUtils.h"
#include "../../include/BoundingBoxes.h"

namespace OpenGLDraw
{
	void drawLine(const Int2& pos1, const Int2& pos2, const Color& color, float thickness);

	void drawRectangle(const Int2& pos1, const Int2& pos2, const Color& color, float borderSize, bool isFilled);

	void drawTexture(int pixelPosX, int pixelPosY, int width,int height);

	void drawFragmentOfTexture(int pixelPosX,int pixelPosY,float x1,float y1,float x2,float y2,int width,int height);

	void drawTextureRotated(
		int pixelPosX,int pixelPosY,
		int width,int height,
		float angleDegree,int rotationCenterX,int rotationCenterY,
		bool mirrorAxisX,bool mirrorAxisY);

	void drawTextureRotated(
		const BoundingBoxes::AdvancedBox& boundingBox,
		bool mirrorAxisX,bool mirrorAxisY);

	void setColor(const Color& color);

	void resetColor();

	void clearScreen(const Color& color);
}

#endif

#endif //OpenGLDraw_h_INCLUDED
