
#include <DirectXMath.h>
#include <wrl\client.h>
#include <agile.h>
#include <math.h>

#include "../../include/FileUtil.h"
#include "../AppSetup.h"
#include "../Texture.h"
#include "DXMain.h"
#include "Picking3D.h"

#include "Scene3DPrivate.h"

//---------------------------------------------------------------------------------------------

Scene3DPrivate::Scene3DPrivate() : m_pos(Float3(0.f, 0.f, 0.f))
{
	//-------------------------------------------
	// constant buffer description

	// Create a constant buffer for passing model, view, and projection matrices
	// to the vertex shader. 

	Assert(DXMain::getDevice() != NULL);

	D3D11_BUFFER_DESC constantBufferDesc = { 0 };
	constantBufferDesc.ByteWidth = sizeof(m_constantBufferData);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;
	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer)
		);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::clearScene(const Color& backgroundColor)
{
	// Clear the render target to a solid color, and reset the depth stencil.
	//const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
	const float clearColor[4] = {
		(float)backgroundColor.r() / 256.f,
		(float)backgroundColor.g() / 256.f,
		(float)backgroundColor.b() / 256.f,
		(float)backgroundColor.a() / 256.f
	};

	DXMain::getContext()->ClearRenderTargetView(DXMain::instance()->m_renderTargetView.Get(), clearColor);
	DXMain::getContext()->ClearDepthStencilView(DXMain::instance()->m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::setPosition(const Float3& pos)
{
	m_pos = pos;

	m_constantBufferData.world = BasicMath::translation(pos.x(), pos.y(), pos.z());

	DXMain::getContext()->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
		);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::setPositionAndScale(const Float3& pos, const Float3& scale_)
{
	m_pos = pos;

	// Update the constant buffer to rotate the cube model.
	float4x4 translationMat = BasicMath::translation(pos.x(), pos.y(), pos.z());
	float4x4 scaleMat = BasicMath::scale(scale_.x(), scale_.y(), scale_.z());
	float4x4 crossMat = BasicMath::mul<float>(translationMat, scaleMat);
	m_constantBufferData.world = crossMat;

	DXMain::getContext()->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
		);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::setRenderOppositeFace(bool yesNo)
{
	//-------------------------------------------
	// rasterizer description

	D3D11_RASTERIZER_DESC rasterizerDesc = DirectXHelper::getRasterizerDesc(AppSetup::instance().isMultisamplingEnabled());

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState)
		);
	DXMain::getContext()->RSSetState(m_RasterizerState.Get());
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::pushPosition()
{
	m_pushedPositions.push_back(m_constantBufferData.world);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::popPosition()
{
	float4x4 crossMat = m_pushedPositions[m_pushedPositions.size() - 1];
	m_pushedPositions.pop_back();

	m_constantBufferData.world = crossMat;

	DXMain::getContext()->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
		);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::setPositionAndScaleAndAngle(const Float3& pos, const Float3& scale_, const Float3& angle)
{
	m_pos = pos;

	// Update the constant buffer to rotate the cube model.
	float4x4 translationMat = BasicMath::translation(pos.x(), pos.y(), pos.z());
	float4x4 scaleMat = BasicMath::scale(scale_.x(), scale_.y(), scale_.z());
	float4x4 crossMat = BasicMath::mul<float>(translationMat, scaleMat);

	float4x4 rotX = BasicMath::rotationX(angle.x());
	float4x4 rotY = BasicMath::rotationY(angle.y());
	float4x4 rotZ = BasicMath::rotationX(angle.z());
	float4x4 rotMat = BasicMath::mul<float>(BasicMath::mul<float>(rotX, rotY), rotZ);
	m_constantBufferData.world = BasicMath::mul<float>(crossMat, rotMat);

	DXMain::getContext()->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
		);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::set3DMode(Camera& camera)
{
	AppSetup::instance().set3DMode(-1.f, -1.f, -1.f);
	
	//int blurLevel = (m_pixelInfoBufferData.blurLevelAndUsesLight % 10);m_pixelInfoBufferData.blurLevelAndUsesLight = (yesNo ? 10 : 0) + blurLevel;

	// Specify the render target and depth stencil we created as the output target.
	DXMain::getContext()->OMSetRenderTargets(
		1,
		DXMain::instance()->m_renderTargetView.GetAddressOf(),
		DXMain::instance()->m_depthStencilView.Get()
		);
	DXMain::instance()->flushContext();
	//this->setPosition(Float3(0.f, 0.f, 0.f));

	float aspectRatio = (float)AppSetup::instance().getWindowCurrentRealSize().width() / (float)AppSetup::instance().getWindowCurrentRealSize().height();

	camera.updateConstantBuffer(&m_constantBufferData, aspectRatio);
	// update the constant buffer with the new data
	DXMain::getContext()->UpdateSubresource(m_constantBuffer.Get(),0,nullptr,&m_constantBufferData,0,0);

	// Update the constant buffer to rotate the cube model.
	//m_constantBufferData.model = rotationY(-m_degree);m_degree += 1.0f;
	m_constantBufferData.world = BasicMath::rotationY(0.f);
}

//---------------------------------------------------------------------------------------------

// This method creates all application resources that depend on
// the application window size.  It is called at app initialization,
// and whenever the application window size changes.
void Scene3DPrivate::createWindowSizeDependentResources()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(
		DXMain::instance()->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer)
		);

	// Cache the rendertarget dimensions in our helper class for convenient use.
	D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
	backBuffer->GetDesc(&backBufferDesc);

	// Finally, update the constant buffer perspective projection parameters
	// to account for the size of the application window.  In this sample,
	// the parameters are fixed to a 70-degree field of view, with a depth
	// range of 0.01 to 100.  See Lesson 5 for a generalized camera class.

	float xScale = 1.42814801f;
	float yScale = 1.42814801f;
	if (backBufferDesc.Width > backBufferDesc.Height)
	{
		xScale = yScale *
			static_cast<float>(backBufferDesc.Height) /
			static_cast<float>(backBufferDesc.Width);
	}
	else
	{
		yScale = xScale *
			static_cast<float>(backBufferDesc.Width) /
			static_cast<float>(backBufferDesc.Height);
	}

	m_constantBufferData.projection = float4x4(
		xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, -0.01f,
		0.0f, 0.0f, -1.0f, 0.0f
		);
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::setWireframeMode(bool yesNo)
{
	//-------------------------------------------
	// rasterizer description

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = yesNo ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;//10;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;//10.f;
	rasterizerDesc.DepthBiasClamp = 0.f;//20.f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = AppSetup::instance().isMultisamplingEnabled();
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	/*if (std::string(textureFile) == std::string("????"))
	{
	rasterizerDesc.DepthBias = -4;
	rasterizerDesc.SlopeScaledDepthBias = -4.0f;
	//rasterizerDesc.DepthBiasClamp = 20.f;
	}*/

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState)
		);
	DXMain::getContext()->RSSetState(m_RasterizerState.Get());
}

//---------------------------------------------------------------------------------------------

void Scene3DPrivate::enableGet3DPosFrom2D(bool yesNo)
{
	Picking3D::enable(yesNo);
}

//---------------------------------------------------------------------------------------------

Float3 Scene3DPrivate::get3DPosFrom2D(const Int2& pos, const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane, const void* givenIndicesAndVerticesOrNull) const
{
	return Picking3D::get3DPosFrom2D(pos, AppSetup::instance().getWindowCurrentRealSize(), 
		m_constantBufferData.projection, m_constantBufferData.world, m_constantBufferData.view, 
		cameraPosition, cameraNearClippingPlane, cameraFarClippingPlane, givenIndicesAndVerticesOrNull);
}

//---------------------------------------------------------------------------------------------

std::vector<Float3> Scene3DPrivate::getAll3DPosFrom2D(const Int2& pos, const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane) const
{
	return Picking3D::getAll3DPosFrom2D(pos, AppSetup::instance().getWindowCurrentRealSize(),
		m_constantBufferData.projection, m_constantBufferData.world, m_constantBufferData.view,
		cameraPosition, cameraNearClippingPlane, cameraFarClippingPlane);
}

//---------------------------------------------------------------------------------------------
