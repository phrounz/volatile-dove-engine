#ifndef Shader_h_INCLUDED
#define Shader_h_INCLUDED

#include <DirectXMath.h>
#include <wrl\client.h>
#include <agile.h>
#include <math.h>

//---------------------------------------------------------------------------------------------

struct VertexInfoBuffer
{
	VertexInfoBuffer() :fogStart(0.f), fogEnd(0.f), windowAspectRatio(1.f), userDefinedTextureId(-1) {}
	float dataX;
	float dataY;
	float dataZ;
	int userDefinedTextureId;
	float fogStart;
	float fogEnd;
	float windowAspectRatio;
	int foo1;
};

struct PixelInfoBuffer
{
	PixelInfoBuffer() : blurLevel(0), usesLight(0) {}
	int blurLevel;
	int usesLight;
	int tmp1;
	int tmp2;
	float4 fogColor;
};

//---------------------------------------------------------------------

class Shader
{
public:
	Shader(bool areShadersSupported);

	static Shader& instance() { return *s_instance; }

	bool isUsing() const { return true; }
	void startUsing();
	void stopUsing() {}

	void injectCameraInfos(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer) { m_constantBuffer = constantBuffer; }

	void setAspectRatio(float value) { m_vertexInfoBufferData.windowAspectRatio = value; }
	void injectMiscData1(float value) { m_vertexInfoBufferData.dataX = value; }
	void injectMiscData2(float value) { m_vertexInfoBufferData.dataY = value; }
	void injectMiscData3(float value) { m_vertexInfoBufferData.dataZ = value; }

	void setFogMinDistance(float dist) { m_vertexInfoBufferData.fogStart = dist; }
	void setFogMaxDistance(float dist) { m_vertexInfoBufferData.fogEnd = dist; }
	void setFogColor(const Color& fogColor) { 
		m_fogColor = fogColor;
		m_pixelInfoBufferData.fogColor = float4(fogColor.r() / 255.f, fogColor.g() / 255.f, fogColor.b() / 255.f, fogColor.a() / 255.f);
	}
	inline float getFogMinDistance() const { return m_vertexInfoBufferData.fogStart; }
	inline float getFogMaxDistance() const { return m_vertexInfoBufferData.fogEnd; }
	inline const Color& getFogColor() const { return m_fogColor; }

	void enableLight(bool yesNo) { m_pixelInfoBufferData.usesLight = (int)yesNo; }
	void injectUserDefinedTextureId(int textureId) { m_vertexInfoBufferData.userDefinedTextureId = textureId; }
	int getCurrentlySelectedUserDefinedTextureId() { return m_vertexInfoBufferData.userDefinedTextureId; }

	//void setRenderBlurLevel(int blurLevel);

	~Shader();

private:
	static Shader* s_instance;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	Color m_fogColor;
	PixelInfoBuffer m_pixelInfoBufferData;
	VertexInfoBuffer m_vertexInfoBufferData;
	//VertexInfoBuffer2 m_vertexInfoBufferData2;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexInfoBuffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexInfoBuffer2;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pixelInfoBuffer;
};

//---------------------------------------------------------------------

#endif //Shader_h_INCLUDED
