
#include "Texture.h"

#include "../include/Material.h"

//---------------------------------------------------------------------

Float4 Material::s_shadowedColor(0.5f, 0.5f, 0.5f, 1.f);

const Float4& Material::getShadowedColorAsFloats()
{
	return s_shadowedColor;
}

Color Material::getShadowedColor()
{
	return Color((u8)(s_shadowedColor.r()*255), (u8)(s_shadowedColor.g()*255), (u8)(s_shadowedColor.b()*255), (u8)(s_shadowedColor.a()*255));
}

void Material::setShadowedColor(const Color& color)
{
	s_shadowedColor = Float4(color.r()/255.f, color.g()/255.f, color.b()/255.f, color.a()/255.f);
}

//---------------------------------------------------------------------

Material::Material() : m_texture(NULL), m_textureShadowed(NULL)
{
	// set the default material
	m_shininess = 65.0f;
	m_diffuse[0] = 0.f;
	m_diffuse[1] = 0.f;
	m_diffuse[2] = 0.8f;
	m_diffuse[3] = 1.0f;
	m_ambient[0] = 0.2f;
	m_ambient[1] = 0.2f;
	m_ambient[2] = 0.2f;
	m_ambient[3] = 1.0f;
	m_specular[0] = 0.0f;
	m_specular[1] = 0.0f;
	m_specular[2] = 0.0f;
	m_specular[3] = 1.0f;
	m_repeat = Float3(1.f, 1.f, 1.f);
}

//---------------------------------------------------------------------

void Material::loadTexture(const std::string& path, float repX, float repY, float repZ, bool alsoAddShadowedTexture)
{
	outputln("Loading material: " << path << " - " << this->getName());
	Utils::indentLog();
	m_texture = new Texture(path.c_str(), NULL);
	if (alsoAddShadowedTexture) m_textureShadowed = new Texture(path.c_str(), &s_shadowedColor);
	m_repeat.data[0] = repX;
	m_repeat.data[1] = repY;
	m_repeat.data[2] = repZ;
	Utils::unindentLog();
}

//---------------------------------------------------------------------

void Material::loadTextureFromMaterialColor(bool alsoAddShadowedTexture)
{
	//outputln("Diffuse color: " << m_diffuse[0] << ',' << m_diffuse[1] << ',' << m_diffuse[2] << ',' << m_diffuse[3]);
	Color colorDiffuse((u8)(m_diffuse[0] * 255), (u8)(m_diffuse[1] * 255), (u8)(m_diffuse[2] * 255), (u8)(m_diffuse[3] * 255));
	outputln("Loading material " << this->getName() << " with color: (" << (int)colorDiffuse.r() << "," << (int)colorDiffuse.g() << "," << (int)colorDiffuse.b() << "," << (int)colorDiffuse.a() << ")");
	Utils::indentLog();
	m_texture = new Texture(colorDiffuse);
	if (alsoAddShadowedTexture)
	{
		Color colorDiffuseShadowed(
			(u8)(m_diffuse[0] * 255 * s_shadowedColor.r()), (u8)(m_diffuse[1] * 255 * s_shadowedColor.g()),
			(u8)(m_diffuse[2] * 255 * s_shadowedColor.b()), (u8)(m_diffuse[3] * 255 * s_shadowedColor.a()));
		m_textureShadowed = new Texture(colorDiffuseShadowed);
	}
	Utils::unindentLog();
}

//---------------------------------------------------------------------

Material::~Material()
{
	outputln("Unloading material " << this->getName());
	Utils::indentLog();
	if (m_texture != NULL) delete m_texture;
	if (m_textureShadowed != NULL) delete m_textureShadowed;
	Utils::unindentLog();
}

//---------------------------------------------------------------------

bool Material::containsTransparency() const
{
	return (m_texture != NULL && m_texture->containsTransparency()) || (m_textureShadowed != NULL && m_textureShadowed->containsTransparency());
}

void Material::setAsCurrentMaterial(bool isShadowed) const
{
	Assert(m_texture != NULL);
	if (isShadowed)
	{
		Assert(m_textureShadowed != NULL);
		m_textureShadowed->setAsCurrentTexture();
	}
	else
	{
		m_texture->setAsCurrentTexture();
	}
}

//---------------------------------------------------------------------
