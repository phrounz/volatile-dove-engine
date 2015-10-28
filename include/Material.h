#ifndef Material_h_INCLUDED
#define Material_h_INCLUDED

#include "../include/CoreUtils.h"

class Texture;

class Material;
typedef const Material* MaterialId;// material identifier

//---------------------------------------------------------------------
/**
* \brief A computer graphics material for a 3D object. Contains one or two textures and lighting informations.
*/

class Material
{
public:

	Material();
	Material(const std::string& path, bool alsoAddShadowedTexture) : m_texture(NULL), m_textureShadowed(NULL)
	{
		this->loadTexture(path, 1.f, 1.f, 1.f, alsoAddShadowedTexture);
	}

	void loadTexture(const std::string& path, float repX, float repY, float repZ, bool alsoAddShadowedTexture);
	void loadTextureFromMaterialColor(bool alsoAddShadowedTexture);

	void setName(const char* paramName) { m_name = paramName; }

	const std::string& getName() const { return m_name; }
	bool hasLoadedTexture() const { return m_texture != NULL; }
	const Float3& getRepeat() const { return m_repeat; }

	void setDiffuse(float diffuse[3])   { memcpy(m_diffuse, diffuse, sizeof(float)*3); }
	void setAmbient(float ambient[3])   { memcpy(m_ambient, ambient, sizeof(float)*3); }
	void setSpecular(float specular[3]) { memcpy(m_specular, specular, sizeof(float)*3); }
	void setShininess(float shininess)  { m_shininess = shininess; }

	const float* getDiffuse() const     { return m_diffuse; }
	const float* getAmbient() const     { return m_ambient; }
	const float* getSpecular() const    { return m_specular; }
	float getShininess() const          { return m_shininess; }

	MaterialId getMaterialId() const { return this; }

	bool containsTransparency() const;

	static const Float4& getShadowedColorAsFloats();
	static Color getShadowedColor();
	static void setShadowedColor(const Color& color);

	void setAsCurrentMaterial(bool isShadowed) const;

	~Material();

private:
	
	float m_diffuse[4];
	float m_ambient[4];
	float m_specular[4];
	float m_emmissive[4];
	float m_shininess;// for specular
	
	Texture* m_texture;
	Texture* m_textureShadowed;
	Float3 m_repeat;
	std::string m_name;

	static Float4 s_shadowedColor;
};

//---------------------------------------------------------------------

#endif //Material_h_INCLUDED
