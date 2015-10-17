
#include "TextureReal.h"

#include "Texture.h"

std::map<std::string,TextureReal*> Texture::m_database;
std::map<TextureReal*, int> Texture::m_counter;

//---------------------------------------------------------------------

Texture::Texture(const char* filepath, const Float4* colorFactorOrNull, bool isShared) :m_isShared(isShared)
{
	if (isShared)
	{
		m_textureReal = getTexture(filepath, colorFactorOrNull);
	}
	else
	{
		if (colorFactorOrNull != NULL)
		{
			outputln("Loading texture: " << filepath << "(" << *colorFactorOrNull << ") (unshared)");
		}
		else
		{
			outputln("Loading texture: " << filepath << ") (unshared)");
		}

		Utils::indentLog();
		m_textureReal = new TextureReal(filepath, colorFactorOrNull, false, true);
		Utils::unindentLog();
	}
}

//---------------------------------------------------------------------

Texture::Texture(const Color& color, bool isShared) :m_isShared(isShared)
{
	if (isShared)
	{
		m_textureReal = getTexture(color);
	}
	else
	{
		outputln("Loading texture (unshared) with color: (" << (int)color.r() << "," << (int)color.g() << "," << (int)color.b() << "," << (int)color.a() << ")");
		Utils::indentLog();
		Image* image = new Image(Int2(16, 16), color);
		m_textureReal = new TextureReal(*image, false, false, true);
		delete image;
		Utils::unindentLog();
	}
}

//---------------------------------------------------------------------

void Texture::setAsCurrentTexture() const
{
	m_textureReal->setAsCurrentTexture();
}

//---------------------------------------------------------------------

bool Texture::containsTransparency() const
{
	return m_textureReal->containsTransparency();
}

//---------------------------------------------------------------------

Texture::~Texture()
{
	if (m_isShared)
	{
		unGetTexture(m_textureReal);
	}
	else
	{
		outputln("Unloading texture: " << m_textureReal->getTextureFilepath() << "(unshared)");
		delete m_textureReal;
	}
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

TextureReal* Texture::getTexture(const char* filepath, const Float4* colorFactorOrNull)
{
	std::string key = filepath + std::string(colorFactorOrNull!=NULL ? std::string("/ MODIFIED COLOR: ") + colorFactorOrNull->toStr() : "");
	std::map<std::string, TextureReal*>::const_iterator it = m_database.find(key);
	TextureReal* texture = NULL;
	if (it == m_database.end())
	{
		outputln("Loading texture: " << key << " (shared)");
		Utils::indentLog();
		texture = new TextureReal(filepath, colorFactorOrNull, false, true);
		Utils::unindentLog();

		m_database[key] = texture;
		m_counter[texture] = 1;
	}
	else
	{
		texture = m_database[key];
		m_counter[texture]++;
	}
	return texture;
}

//---------------------------------------------------------------------

TextureReal* Texture::getTexture(const Color& color)
{
	std::stringstream keystr;
	keystr << std::string("2;") << color.r() << "," << color.g() << "," << color.b() << "," << color.a();
	std::string key = keystr.str();
	std::map<std::string, TextureReal*>::const_iterator it = m_database.find(key);
	TextureReal* texture = NULL;
	if (it == m_database.end())
	{
		outputln("Loading texture with color: (" << (int)color.r() << "," << (int)color.g() << "," << (int)color.b() << "," << (int)color.a() << ") (shared)");
		Image* image = new Image(Int2(16, 16), color);
		texture = new TextureReal(*image, false, false, true);
		delete image;

		m_database[key] = texture;
		m_counter[texture] = 1;
	}
	else
	{
		texture = m_database[key];
		m_counter[texture]++;
	}
	return texture;
}

//---------------------------------------------------------------------

void Texture::unGetTexture(TextureReal* texture)
{
	m_counter[texture]--;
	if (m_counter[texture] == 0)
	{
		iterateMapConst(m_database, std::string, TextureReal*)
		{
			if ((*it).second == texture)
			{
				outputln("Unloading texture: " << texture->getTextureFilepath() << " (shared)");
				delete texture;
				m_database.erase((*it).first);
				return;
			}
		}
	}
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
