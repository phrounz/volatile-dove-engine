
#if defined(USES_WINDOWS_OPENGL)
	#include <windows.h>
	#include <gl/GL.h>
	#include <gl/GLU.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
#elif defined (USES_LINUX)
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
#else
	#error
#endif

#include "OpenGLDraw.h"

namespace OpenGLDraw
{

//---------------------------------------------------------------------

//static Color previousColor(colorWhite);

//---------------------------------------------------------------------

void drawLine(const Int2& pos1, const Int2& pos2, const Color& color, float thickness)
{
	glLineWidth(thickness);
	glColor4f(color.r()/256.f, color.g()/256.f, color.b()/256.f, color.a()/256.f);
	glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);

	glBegin(GL_LINES);
    glVertex2i(pos1.x(), pos1.y());
    glVertex2i(pos2.x(), pos2.y());
    glEnd();
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

//---------------------------------------------------------------------
//	Draw a rectangle

void drawRectangle(const Int2& pos1, const Int2& pos2, const Color& color, float borderSize, bool isFilled)
{
	glLineWidth(borderSize);
	glColor4f(color.r()/256.f, color.g()/256.f, color.b()/256.f, color.a()/256.f);
	//glColor4f(color.r()/255.f, color.g()/255.f, color.b()/255.f, color.a()/255.f);
	glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);

    glBegin(isFilled ? GL_QUADS : GL_LINE_LOOP);
    glVertex2i(pos1.x(), pos1.y());
    glVertex2i(pos2.x(), pos1.y());
    glVertex2i(pos2.x(), pos2.y());
    glVertex2i(pos1.x(), pos2.y());
    glEnd();
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

//---------------------------------------------------------------------

void drawTexture(int pixelPosX, int pixelPosY, int width,int height)
{
	glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, (GLuint)textureId );

    glMatrixMode(GL_MODELVIEW);

	glBegin(GL_POLYGON);

    glTexCoord2i(1, 1);//1,0
	glVertex2i(pixelPosX+width, pixelPosY+height);

    glTexCoord2i(0, 1);//0,0
	glVertex2i(pixelPosX, pixelPosY+height);

    glTexCoord2i(0, 0);//0,1
	glVertex2i(pixelPosX, pixelPosY);

    glTexCoord2i(1, 0);//1,1
	glVertex2i(pixelPosX+width, pixelPosY);

	glEnd();
}

//---------------------------------------------------------------------
//	Display a part of a texture loaded in video memory
//	x1,y1,x2,y2 are values between 0 and 1
//	Note: it uses the current color for rendering the texture
//

void drawFragmentOfTexture(int pixelPosX,int pixelPosY,float x1,float y1,float x2,float y2,int width,int height)
{
    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, (GLuint)id );

    //std::cout << id << std::endl;
    glMatrixMode(GL_MODELVIEW);

	glBegin(GL_POLYGON);

    //glColor4ub(255, 255, 0, 0);
	glTexCoord2f(x2, y2);//1,0
	glVertex2i(pixelPosX+width, pixelPosY+height);

    //glColor4ub(255, 255, 0, 0);
	glTexCoord2f(x1, y2);//0,0
	glVertex2i(pixelPosX, pixelPosY+height);

    //glColor4ub(255, 255, 0, 0);
	glTexCoord2f(x1, y1);//0,1
	glVertex2i(pixelPosX, pixelPosY);

    //glColor4ub(255, 255, 0, 0);
	glTexCoord2f(x2, y1);//1,1
	glVertex2i(pixelPosX+width, pixelPosY);

	glEnd();
}

//---------------------------------------------------------------------
//	Display a texture loaded in video memory
//	Also supports mirror, scaling, rotation, effects
//	Note: it uses the current color for rendering the texture
//	

void drawTextureRotated(
	int pixelPosX,int pixelPosY,
	int width,int height,
	float angleDegree,int rotationCenterX,int rotationCenterY,
	bool mirrorAxisX,bool mirrorAxisY)
{
	BoundingBoxes::AdvancedBox advancedBox;
    advancedBox.calculate(Int2(pixelPosX, pixelPosY), Int2(width, height), Int2(rotationCenterX, rotationCenterY), angleDegree);

    drawTextureRotated(advancedBox, mirrorAxisX, mirrorAxisY);
}

//---------------------------------------------------------------------
//	Display a texture loaded in video memory
//	and use its bounding box to display it with scaling, rotation
//  Also supports mirror effect.
//	Note: it uses the current color for rendering the texture
//	

void drawTextureRotated(
	const BoundingBoxes::AdvancedBox& boundingBox,
	bool mirrorAxisX,bool mirrorAxisY)
{
    //if (!mirrorAxisX && !mirrorAxisY && angleDegree==0.0f)
    //    return drawTexture(id,pixelPosX-rotationCenterX,pixelPosY-rotationCenterY,width,height);

	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, (GLuint)id);

    glMatrixMode(GL_MODELVIEW);
	glBegin(GL_POLYGON);

	glTexCoord2i(mirrorAxisX?0:1, mirrorAxisY?0:1);
	glVertex2i(boundingBox.data[0].x(), boundingBox.data[0].y());

	glTexCoord2i(mirrorAxisX?1:0, mirrorAxisY?0:1);
	glVertex2i(boundingBox.data[1].x(), boundingBox.data[1].y());

	glTexCoord2i(mirrorAxisX?1:0, mirrorAxisY?1:0);
	glVertex2i(boundingBox.data[2].x(), boundingBox.data[2].y());

	glTexCoord2i(mirrorAxisX?0:1, mirrorAxisY?1:0);
	glVertex2i(boundingBox.data[3].x(), boundingBox.data[3].y());

	glEnd();
}

//---------------------------------------------------------------------

void setColor(const Color& color)
{
	//float col[4];
	//glGetFloatv(GL_CURRENT_COLOR, col);
	//previousColor = Color(col[0] * 256.f, col[1] * 256.f, col[2] * 256.f, col[3] * 256.f);

	glColor4ub(color.r(), color.g(), color.b(), color.a());
}

//---------------------------------------------------------------------

void resetColor()
{
	glColor4ub(CoreUtils::colorWhite.r(), CoreUtils::colorWhite.g(), CoreUtils::colorWhite.b(), CoreUtils::colorWhite.a());
}

//---------------------------------------------------------------------

void OpenGLDraw::clearScreen(const Color& color)
{
	glClearColor(color.r() / 255.f, color.g() / 255.f, color.b() / 255.f, color.a() / 255.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//---------------------------------------------------------------------

} //namespace OpenGLDraw
