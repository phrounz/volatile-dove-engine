#ifndef Texture_h_INCLUDED
#define Texture_h_INCLUDED

#include <string>
#include <map>
#include <utility>

#include "../include/CoreUtils.h"
#include "../include/Utils.h"
#include "../include/Image.h"

class TextureReal;

//---------------------------------------------------------------------

class Texture
{
public:
	Texture(const char* filepath, const Float4* colorFactorOrNull, bool isShared = true);
	Texture(const Color& color, bool isShared = true);

	void setAsCurrentTexture() const;
	bool containsTransparency() const;

	~Texture();

	//const TextureReal* getTextureReal() const { return m_textureReal; }

private:
	
	bool m_isShared;
	TextureReal* m_textureReal;

	static TextureReal* getTexture(const char* filepath, const Float4* colorFactorOrNull);
	static TextureReal* getTexture(const Color& color);
	static void unGetTexture(TextureReal* texture);

	static std::map<TextureReal*, int> m_counter;
	static std::map<std::string, TextureReal*> m_database;
};

//---------------------------------------------------------------------

#endif //Texture_h_INCLUDED
