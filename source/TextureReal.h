#ifndef TextureReal_h_INCLUDED
#define TextureReal_h_INCLUDED

#include <string>

#include "../include/CoreUtils.h"

struct TextureImpl;

class Image;

//---------------------------------------------------------------------

class TextureReal
{
public:

	TextureReal(const Image& image, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap);// use clampToEdge=true for 2D, false for 3D

	TextureReal(const char* textureFile, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap);
	
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	TextureReal(int width, int height, const unsigned char* data, bool isAlpha, bool containsTransparency, bool clampToEdge, bool useMipmap);
#endif

	void setAsCurrentTexture() const;
	bool containsTransparency() const;
	const std::string& getTextureFilepath() const { return m_textureFile; }

	~TextureReal();
private:
	void createTexture(const Image& image, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap);
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	void createTexture(int width, int height, const unsigned char* data, bool isAlpha, bool containsTransparency, bool clampToEdge, bool useMipmap);
#endif
	TextureImpl* textureImpl;
	std::string m_textureFile;
};

//---------------------------------------------------------------------

#endif //TextureReal_h_INCLUDED
