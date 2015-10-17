#ifndef Scene3DPrivate_h_INCLUDED
#define Scene3DPrivate_h_INCLUDED

/******************************************************************
Done by Francois Braud, with help of
http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
******************************************************************/

#include <wrl.h>
#include <d3d11_1.h>
#include <agile.h>
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;

#include "../../include/Camera.h"

#include "../ConstantBuffer.h"

class VBO;
class Texture;

//---------------------------------------------------------------------------------------------

class Scene3DPrivate
{
public:
	
	Scene3DPrivate();

	bool areShadersSupported() const { return true; }

	void clearScene(const Color& backgroundColor);
	void setPosition(const Float3& pos);
	void setPositionAndScale(const Float3& pos, const Float3& scale_);
	void setRenderOppositeFace(bool yesNo);
	void pushPosition();
	void popPosition();
	void setPositionAndScaleAndAngle(const Float3& pos, const Float3& scale_, const Float3& angle);
	void set3DMode(Camera& camera);
	void createWindowSizeDependentResources();
	void setWireframeMode(bool yesNo);
	void enableGet3DPosFrom2D(bool yesNo);
	Float3 get3DPosFrom2D(const Int2& pos, const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane, const void* givenIndicesAndVerticesOrNull) const;
	std::vector<Float3> Scene3DPrivate::getAll3DPosFrom2D(const Int2& pos, const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane) const;

	~Scene3DPrivate() { m_constantBuffer.Reset(); m_RasterizerState.Reset(); }

	Microsoft::WRL::ComPtr<ID3D11Buffer> getCameraInfos() { return m_constantBuffer; }

	Float3 getPosition() { return m_pos; }

private:

	Float3 m_pos;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;

	ConstantBuffer m_constantBufferData;

	std::vector<float4x4> m_pushedPositions;
};

//---------------------------------------------------------------------------------------------

#endif //Scene3DPrivate_h_INCLUDED
