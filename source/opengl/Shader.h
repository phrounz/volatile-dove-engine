#ifndef Shader_h_INCLUDED
#define Shader_h_INCLUDED

#include "../../include/CoreUtils.h"
#include "../../include/BasicMath.h"

//---------------------------------------------------------------------

class Shader
{
public:
	Shader(bool areShadersSupported);

	static Shader& instance() { return *s_instance; }

	void startUsing();
	void stopUsing();
	bool isUsing() const { return m_isUsing; }
	
	void injectCameraInfos(const float mat[16]) { memcpy(m_cameraMatrix, mat, 16 * sizeof(float)); }

	void setAspectRatio(float value) {}
	void injectMiscData1(float value) { m_data.data[0] = value; }
	void injectMiscData2(float value) { m_data.data[1] = value; }
	void injectMiscData3(float value) { m_data.data[2] = value; }

	void setFogMinDistance(float dist) { m_fogStart = dist; }
	void setFogMaxDistance(float dist) { m_fogEnd = dist; }
	void setFogColor(const Color& fogColor) { m_fogColor = fogColor; }
	inline float getFogMinDistance() const { return m_fogStart; }
	inline float getFogMaxDistance() const { return m_fogEnd; }
	inline const Color& getFogColor() { return m_fogColor; }

	void enableLight(bool yesNo) { m_lightEnabled = yesNo; }

	void injectUserDefinedTextureId(int textureId) { m_userDefinedTextureId = textureId; }
	int getCurrentlySelectedUserDefinedTextureId() { return m_userDefinedTextureId; }

	~Shader();
private:
	static Shader* s_instance;

	bool m_areShadersSupported;

	Float3 m_data;
	Color m_fogColor;
	float m_fogStart;
	float m_fogEnd;
	bool m_lightEnabled;
	float m_cameraMatrix[16];
	int m_userDefinedTextureId;

	bool compileShaders(const char* vertexShaderSource, int vertexShaderLength, const char* fragmentShaderSource, int fragmentShaderLength);
	unsigned int m_vertexShaderObject;
	unsigned int m_fragmentShaderObject;
	unsigned int m_programObject;

	bool m_isUsing;
};

//---------------------------------------------------------------------

#endif //Shader_h_INCLUDED
