
#if defined(USES_WINDOWS_OPENGL)
	#include <windows.h>
	#include <gl/GL.h>
	#include <gl/GLU.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
	#include <GL/glext.h>
#elif defined (USES_LINUX)
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#ifndef USES_JS_EMSCRIPTEN
		#include <GL/freeglut_ext.h>
	#endif
#endif

#include "../TextureReal.h"

#include "../../include/Engine.h"
#include "../../include/CoreUtils.h"
#include "../../include/Utils.h"
#include "../../include/Image.h"
#include "../../include/FileUtil.h"

namespace
{
	const bool FORCE_USE_MIPMAP_FOR_OLD_VERSIONS = true;
}

//---------------------------------------------------------------------

struct TextureImpl
{
	unsigned int m_textureId;
	bool m_containsTransparency;
};

//---------------------------------------------------------------------------------------------

TextureReal::TextureReal(const Image& image, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap)
:textureImpl(new TextureImpl()), m_textureFile(colorFactorOrNull?"<not a file (standard)>":"<not a file (modified)>")
{
	this->createTexture(image, colorFactorOrNull, clampToEdge, useMipmap);
}

//---------------------------------------------------------------------------------------------

TextureReal::TextureReal(const char* textureFile, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap)
:textureImpl(new TextureImpl()), m_textureFile(textureFile)
{
	//outputln("Creating texture from " << textureFile);
	Image* image = new Image(textureFile);
	this->createTexture(*image, colorFactorOrNull, clampToEdge, useMipmap);
	delete image;
}

//---------------------------------------------------------------------

TextureReal::TextureReal(int width, int height, const unsigned char* data, bool isAlpha, bool containsTransparency, bool clampToEdge, bool useMipmap)
{
	this->createTexture(width, height, data, isAlpha, containsTransparency, clampToEdge, useMipmap);
}

//---------------------------------------------------------------------------------------------

bool TextureReal::containsTransparency() const
{
	return textureImpl->m_containsTransparency;
}

//---------------------------------------------------------------------

//load an image into the graphic card
void TextureReal::createTexture(const Image& image, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap)
{
	Image* imageAltered = NULL;
	if (colorFactorOrNull != NULL)
	{
		imageAltered = new Image(image, *colorFactorOrNull);
	}
	this->createTexture(image.size().width(), image.size().height(), image.getDataPixels(), image.getIsAlpha(), image.containsTransparency(), clampToEdge, useMipmap);
	if (colorFactorOrNull != NULL)
	{
		delete imageAltered;
	}
}

//---------------------------------------------------------------------

void TextureReal::createTexture(int width, int height, const unsigned char* data, bool isAlpha, bool containsTransparency, bool clampToEdge, bool useMipmap)
{
	if (FORCE_USE_MIPMAP_FOR_OLD_VERSIONS) // fix a bug occured with an old intel graphic card on vista with OpenGL 1.5
	{
		const GLubyte* version = glGetString(GL_VERSION);
		if (strcmp((const char*)version, "2.0") < 0) // put it for any program with OpenGL version less than 2.0
		{
			outputln("Force use mipmap");
			useMipmap = true;
		}
	}
	Assert(data != NULL);

    //const bool linearRendering = true;
    GLuint idTexture = 0;

	//texture generation
	glGenTextures(1, &idTexture );
	
    glBindTexture(GL_TEXTURE_2D, idTexture );

	//if (useMipmap)
	//{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampToEdge ? GL_CLAMP_TO_EDGE :  GL_CLAMP_TO_EDGE | GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampToEdge ? GL_CLAMP_TO_EDGE :  GL_CLAMP_TO_EDGE | GL_REPEAT);
	//}
	//else
	//{
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	//}
	
	// when texture area is small, bilinear filter the closest mipmap
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    // when texture area is large, bilinear filter the first mipmap
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	//set tabTexture data for the new texture
	if (useMipmap)
	{
		int mipmapres = gluBuild2DMipmaps(GL_TEXTURE_2D,isAlpha?GL_RGBA:GL_RGB, width,height,isAlpha?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,data);
		if (mipmapres != 0)
		{
			std::string err = (const char*)(gluErrorString(mipmapres));
			AssertMessage(false, std::string("gluBuild2DMipmaps:") + err.c_str());
		}
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D,0,isAlpha?GL_RGBA:GL_RGB,width,height,0,isAlpha?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,data);
	}

	textureImpl->m_textureId = (unsigned int)idTexture;
	textureImpl->m_containsTransparency = containsTransparency;
}

//---------------------------------------------------------------------

void TextureReal::setAsCurrentTexture() const
{
	Engine::instance().getScene3DMgr().useUserDefinedTextureIdIfAvailable(FileUtil::basename(m_textureFile));

	const GLuint tmpgluint = (GLuint)textureImpl->m_textureId;
	glBindTexture(GL_TEXTURE_2D, (GLuint)tmpgluint );
}

//---------------------------------------------------------------------

TextureReal::~TextureReal()
{
	const GLuint tmpgluint = (GLuint)textureImpl->m_textureId;
    glDeleteTextures((GLsizei)1,&tmpgluint);
}

//---------------------------------------------------------------------
