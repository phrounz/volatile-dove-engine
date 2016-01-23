
//-------------------------------------------------------------------------

#include "../include/global_defines.h"

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	#include "opengl/OpenGLDraw.h"
	#include "opengl/SDLDraw.h"
	#include "TextureReal.h"
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	#include "directx/DXMain.h"
#endif

#include "../include/Image.h"

//-------------------------------------------------------------------------

#include "../include/Bitmap.h"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)

namespace
{
	const bool ENABLE_ROUNDED_REDUCED_IMAGE = false;
}

struct D2D1_SIZE_U
{
	unsigned int width;
	unsigned int height;
};

#ifdef USES_SDL_INSTEAD_OF_GLUT
SDL_Surface* SDL_CreateRGBASurfaceFrom(void* buffer, int width, int height)
{
	/*m_surface = SDL_CreateRGBASurface(m_size.width,m_size.height);
	SDL_LockSurface(m_surface);
	memcpy(m_surface->pixels, imageBuffer, m_surface->w * m_surface->h);
	SDL_UnlockSurface(m_surface);*/

	unsigned int rmask = 0, gmask = 0, bmask = 0, amask = 0;
	if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
	{
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	}
	else
	{
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	}
	return SDL_CreateRGBSurfaceFrom(buffer, width, height, 32, width*4, rmask, gmask, bmask, amask);
}
#endif

struct ID2D1Bitmap
{
	ID2D1Bitmap(const D2D1_SIZE_U& parSize, bool useMipmap)
		: m_size(parSize), m_useMipmap(useMipmap)
#ifdef USES_SDL_INSTEAD_OF_GLUT
		, m_surface(NULL) //, m_textureReduced(NULL)
#else
		, m_texture(NULL) //, m_textureReduced(NULL)
#endif
	{
		m_buffer = new unsigned char[m_size.width*m_size.height*4];
	}
	void CopyFromMemory(const unsigned char* imageBuffer)
	{
		Assert(m_buffer != NULL);
		memcpy(m_buffer, imageBuffer, m_size.width*m_size.height*4);
#ifdef USES_SDL_INSTEAD_OF_GLUT
		m_surface = SDL_CreateRGBASurfaceFrom((void*)imageBuffer, m_size.width, m_size.height);
		m_texture = SDLDraw::createTextureFromSurface(m_surface);

#else
		Assert(m_texture == NULL);
		m_texture = new TextureReal(m_size.width, m_size.height, m_buffer, true, true, false, m_useMipmap);
#endif
	}
	void copyFromImage(const Image* image)
	{
#ifdef USES_SDL_INSTEAD_OF_GLUT
		m_surface = SDL_CreateRGBASurfaceFrom((void*)image->getDataPixels(), m_size.width, m_size.height);
		m_texture = SDLDraw::createTextureFromSurface(m_surface);

#else
		//Image* imageNew = new Image(*image, 1);
		m_texture = new TextureReal(*image, NULL, true, m_useMipmap);
		//delete imageNew;
#endif
	}
	~ID2D1Bitmap()
	{
#ifdef USES_SDL_INSTEAD_OF_GLUT
		SDL_FreeSurface(m_surface);
		//SDL_FreeTexture(m_texture);
#else
		delete m_texture;
#endif
		if (m_buffer != NULL) {delete m_buffer;m_buffer = NULL;}
	}
	void freeBuffer()
	{
		if (m_buffer != NULL) {delete m_buffer;m_buffer = NULL;}
	}
#ifdef USES_SDL_INSTEAD_OF_GLUT
	const SDL_Texture* getSDLSurface() const { return m_texture; }
#else
	void setAsCurrentTexture() const { m_texture->setAsCurrentTexture(); }
#endif
	const D2D1_SIZE_U& GetSize() {return m_size;}
private:
	unsigned char* m_buffer;
	D2D1_SIZE_U m_size;
#ifdef USES_SDL_INSTEAD_OF_GLUT
	SDL_Surface* m_surface;
	SDL_Texture* m_texture;
#else
	TextureReal* m_texture;
	//TextureReal* m_textureReduced;
#endif
	bool m_useMipmap;
};

#endif

//-------------------------------------------------------------------------

Bitmap::Bitmap(const char* fileName):m_fileName(fileName), m_isAlpha(true),m_bitmapSize(-1,-1)
{
	outputln("Loading bitmap: " << fileName);
	Utils::indentLog();
    Image* image = new Image(fileName, false);
	this->initFromImage(*image, false);
	delete image;
	Utils::unindentLog();
}

//-------------------------------------------------------------------------

Bitmap::Bitmap(unsigned char* fileBuffer, unsigned int fileSize):m_isAlpha(true)
{
	// use an image
	outputln("Loading bitmap from buffer (" << fileSize << " bytes)");
	Utils::indentLog();
	Image* image = new Image(fileBuffer, fileSize);
	this->initFromImage(*image, false);
	delete image;
	Utils::unindentLog();
}

//-------------------------------------------------------------------------

Bitmap::Bitmap(const Image& image, bool useMipmap)
{
	outputln("Loading bitmap from image");
	Utils::indentLog();
	this->initFromImage(image, useMipmap);
	Utils::unindentLog();
}

//-------------------------------------------------------------------------

void Bitmap::initFromImage(const Image& image, bool useMipmap)
{
	m_isAlpha = image.getIsAlpha();

	// init bitmap
	m_bitmapSize = image.size();
	D2D1_SIZE_U sizeBitmap;
	sizeBitmap.width = image.size().width();
	sizeBitmap.height = image.size().height();
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_bitmap = new ID2D1Bitmap(sizeBitmap, useMipmap);
#else
	m_bitmap = DXMain::instance()->createBitmap(sizeBitmap);
#endif

	// copy from buffer
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_bitmap->copyFromImage(&image);
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	this->copyFromBuffer(Int2(0,0), image.size(), image.getDataPixels());
#endif

	// image buffer is no more required
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_bitmap->freeBuffer();
#endif
}

//-------------------------------------------------------------------------
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

unsigned char* convertToPremultiplied(const Int2& size, const unsigned char* src)
{
	unsigned char* dest = new unsigned char[size.width() * size.height() * 4];

	int countTransparentPixels = 0;
	//convert D2D1_ALPHA_MODE_STRAIGHT to D2D1_ALPHA_MODE_PREMULTIPLIED
	int ptr = 0;

	memcpy(dest, src, size.width() * size.height() * 4);

	for (int i = 0; i < size.width(); ++i)
	{
		for (int j = 0; j < size.height(); ++j)
		{
			const unsigned char* pixelSrc = &src[ptr];
			unsigned char* pixelDestination = &dest[ptr];
			unsigned char alpha = pixelSrc[3];
			if (alpha < 250)
			{
				countTransparentPixels++;
			}
			float alphaFactor = (float)alpha / 255.f;
			int pixelTmp[3];
			for (int k = 0; k < 3; ++k)
			{
				pixelTmp[k] = (int)((float)pixelSrc[k] * alphaFactor);
				if (pixelTmp[k] < 0) pixelTmp[k] = 0;
				else if (pixelTmp[k] > 255) pixelTmp[k] = 255;
				pixelDestination[k] = (unsigned char)pixelTmp[k];
			}
			ptr += 4;
		}
	}
	//m_containsTransparency = (float)countTransparentPixels / (float)(width*height) > 0.01f;

	return dest;
}
#endif
//-------------------------------------------------------------------------

void Bitmap::copyFromBuffer(const Int2& posInBitmap, const Int2& size, const unsigned char* src)
{
	// copy from buffer
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_bitmap->CopyFromMemory(src);
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	D2D1_RECT_U rect;
	rect.left = posInBitmap.x();
	rect.top = posInBitmap.y();
	rect.right = posInBitmap.x() + size.width();
	rect.bottom = posInBitmap.y() + size.height();
	unsigned char* premultipliedSrc = convertToPremultiplied(size, src);
	m_bitmap->CopyFromMemory(&rect, premultipliedSrc, size.width() * 4);
	delete [] premultipliedSrc;
#else
	#error
#endif

	// image buffer is no more required
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	m_bitmap->freeBuffer();
#endif
}

//-------------------------------------------------------------------------

Int2 Bitmap::size() const
{
	return m_bitmapSize;
}

//-------------------------------------------------------------------------

void Bitmap::draw(const Int2& pos, float opacity, const Float2& scale) const
{
#ifdef USES_SDL_INSTEAD_OF_GLUT
	SDLDraw::drawTexture(m_bitmap->getSDLSurface(), pos.x(), pos.y(), (int)(this->size().width()*scale.x()), (int)(this->size().height()*scale.y()));
#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	if (opacity != 1.f) OpenGLDraw::setColor(Color(255,255,255,(u8)(255*opacity)));
	m_bitmap->setAsCurrentTexture();
	OpenGLDraw::drawTexture(pos.x(), pos.y(), (int)(this->size().width()*scale.x()), (int)(this->size().height()*scale.y()));
	if (opacity != 1.f) OpenGLDraw::resetColor();
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	DXMain::instance()->drawBitmap(m_bitmap, CoreUtils::fromInt2ToFloat2(pos), scale, opacity, 0, 0, 0.f, Int2(0, 0), false, false);

#endif
}

//-------------------------------------------------------------------------

void Bitmap::drawFragment(const Int2& pos, float x1,float y1,float x2,float y2, int width,int height, float opacity) const
{
#if defined(USES_SDL_INSTEAD_OF_GLUT)
	SDLDraw::drawFragmentOfTexture(m_bitmap->getSDLSurface(), pos.x(),pos.y(),x1,y1,x2,y2,width,height, opacity);
#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	if (opacity != 1.f) OpenGLDraw::setColor(Color(255,255,255,(u8)(255*opacity)));
	m_bitmap->setAsCurrentTexture();
	OpenGLDraw::drawFragmentOfTexture(pos.x(),pos.y(),x1,y1,x2,y2,width,height);
	if (opacity != 1.f) OpenGLDraw::resetColor();
#elif defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	D2D1_RECT_F rectSource;
	rectSource.left   = x1 * this->size().width();
	rectSource.top    = y1 * this->size().height();
	rectSource.right  = x2 * this->size().width();
	rectSource.bottom = y2 * this->size().height();

	D2D1_RECT_F rectDest = D2D1::RectF(0.f, 0.f, (float)width, (float)height);

	DXMain::instance()->drawBitmap(m_bitmap, CoreUtils::fromInt2ToFloat2(pos), Float2(1.f, 1.f), opacity, &rectSource, &rectDest, 0.f, Int2(0, 0), false, false);

#endif
}

//-------------------------------------------------------------------------

void Bitmap::drawRotated(const Int2& pixelPos, const Int2& size, float angleDegree, const Int2& rotationCenterPos, bool mirrorAxisX, bool mirrorAxisY, float opacity) const
{
#if defined(USES_SDL_INSTEAD_OF_GLUT)
	SDLDraw::drawTextureRotated(m_bitmap->getSDLSurface(), pixelPos.x(), pixelPos.y(), size.x(), size.y(), angleDegree, rotationCenterPos.x(), rotationCenterPos.y(), mirrorAxisX, mirrorAxisY);
#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	if (opacity != 1.f) OpenGLDraw::setColor(Color(255,255,255,(u8)(255*opacity)));
	m_bitmap->setAsCurrentTexture();
	OpenGLDraw::drawTextureRotated(pixelPos.x(), pixelPos.y(), size.x(), size.y(), angleDegree, rotationCenterPos.x(), rotationCenterPos.y(), mirrorAxisX, mirrorAxisY);
	if (opacity != 1.f) OpenGLDraw::resetColor();
#else
	//Assert(!mirrorAxisX && !mirrorAxisY);
	Float2 scale((float)size.x() / (float)this->size().x() * (mirrorAxisX ? -1.f : 1.f), (float)size.y() / (float)this->size().y() * (mirrorAxisY ? -1.f : 1.f));
	DXMain::instance()->drawBitmap(m_bitmap, CoreUtils::fromInt2ToFloat2(pixelPos), scale, opacity, 0, 0, angleDegree, rotationCenterPos, mirrorAxisX, mirrorAxisY);
#endif
}

//-------------------------------------------------------------------------

void Bitmap::drawRotated(const BoundingBoxes::AdvancedBox& boundingBox, bool mirrorAxisX, bool mirrorAxisY, float opacity) const
{
#if defined(USES_SDL_INSTEAD_OF_GLUT)
	#pragma message("TODO Bitmap::drawRotated")
#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	if (opacity != 1.f) OpenGLDraw::setColor(Color(255,255,255,(u8)(255*opacity)));
	m_bitmap->setAsCurrentTexture();
	if (opacity != 1.f) OpenGLDraw::resetColor();
#else
	#pragma message("TODO Bitmap::drawRotated")
#endif
}

//-------------------------------------------------------------------------

Bitmap::~Bitmap()
{
	if (m_fileName != "")
	{
		outputln("Unloading bitmap: " << m_fileName);
	}
	else
	{
		outputln("Unloading bitmap");
	}
	Utils::indentLog();

	#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
		delete m_bitmap;
	#else
		m_bitmap->Release();
		m_bitmap = NULL;
		//delete m_bitmap;
	#endif
	Utils::unindentLog();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
