
#include "../../include/FileUtil.h"
#include "../Texture.h"
#include "../AppSetup.h"
#include "DXMain.h"
#include "DirectXHelper.h"
#include "Picking3D.h"

#include "Shader.h"

//---------------------------------------------------------------------------------------------

Shader* Shader::s_instance = NULL;

//---------------------------------------------------------------------------------------------

Shader::Shader(bool areShadersSupported)
{
	s_instance = this;

	Utils::indentLog("Initializing shaders ...");

	Assert(areShadersSupported);
	
	//-------------------------------------------
	// vertex shader and layout description
	
	// Load the raw vertex shader bytecode from disk and create a vertex shader with it.
	auto vertexShaderBytecode = FileUtil::readFile(FileUtil::APPLICATION_FOLDER, "./shaders/SimpleVertexShader.cso");

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateVertexShader(
		vertexShaderBytecode->Data,
		vertexShaderBytecode->Length,
		nullptr,
		&m_vertexShader
		)
		);
	
	// Create an input layout that matches the layout defined in the vertex shader code.
	// These correspond to the elements of the BasicVertex struct defined above.
	const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateInputLayout(
			basicVertexLayoutDesc,
			ARRAYSIZE(basicVertexLayoutDesc),
			vertexShaderBytecode->Data,
			vertexShaderBytecode->Length,
			&m_inputLayout
		)
		);

	delete vertexShaderBytecode;

	//-------------------------------------------
	// pixel shader

	// Load the raw pixel shader bytecode from disk and create a pixel shader with it.
	auto pixelShaderBytecode = FileUtil::readFile(FileUtil::APPLICATION_FOLDER, "./shaders/SimplePixelShader.cso");

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreatePixelShader(
		pixelShaderBytecode->Data,
		pixelShaderBytecode->Length,
		nullptr,
		&m_pixelShader
		)
		);

	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476092%28v=vs.85%29.aspx
	// buffer must be multiple of 16 bytes and < 4096

	//-------------------------------------------
	// constant vertex buffer description

	D3D11_BUFFER_DESC bufDesc = { 0 };
	bufDesc.ByteWidth = sizeof(m_vertexInfoBufferData);
	Assert(bufDesc.ByteWidth % 16 == 0);
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;//D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;//D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//0;
	bufDesc.StructureByteStride = 0;
	
	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_vertexInfoBufferData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateBuffer(
			&bufDesc,
			&initData,
			&m_vertexInfoBuffer
			)
		);
	
	//-------------------------------------------
	// constant pixel info buffer description

	D3D11_BUFFER_DESC pixelInfoDesc = { 0 };
	pixelInfoDesc.ByteWidth = sizeof(m_pixelInfoBufferData);
	//Assert(pixelInfoDesc.ByteWidth == 32);
	pixelInfoDesc.Usage = D3D11_USAGE_DYNAMIC;//D3D11_USAGE_DEFAULT;
	pixelInfoDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelInfoDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelInfoDesc.MiscFlags = 0;//D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;//0;
	pixelInfoDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData2;
	initData2.pSysMem = &m_pixelInfoBufferData;
	initData2.SysMemPitch = 0;
	initData2.SysMemSlicePitch = 0;

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateBuffer(
		&bufDesc,
		&initData,
		&m_pixelInfoBuffer
		)
		);

	//-------------------------------------------
	// blend description

	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	for (int i = 0; i < 1; ++i)
	{
		D3D11_RENDER_TARGET_BLEND_DESC& rtbd = desc.RenderTarget[i];

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;//SRC_ALPHA;//D3D11_BLEND_ONE;//SRC_COLOR;//ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;//INV_SRC_ALPHA;//D3D11_BLEND_ZERO;//INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;//SRC_ALPHA;//_ONE;//;//ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;//DEST_ALPHA;//ZERO;//INV_SRC_ALPHA;//DEST_ALPHA;//_ZERO;//ONE;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		/*rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;//SRC_ALPHA;//D3D11_BLEND_ONE;//SRC_COLOR;//ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;//INV_SRC_ALPHA;//D3D11_BLEND_ZERO;//INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;//SRC_ALPHA;//_ONE;//;//ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;//DEST_ALPHA;//ZERO;//INV_SRC_ALPHA;//DEST_ALPHA;//_ZERO;//ONE;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;*/
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;// 0x0f;
	}

	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateBlendState(&desc, &m_blendState)
		);

	Utils::unindentLog("Initializing shaders done.");
}

//---------------------------------------------------------------------------------------------

void Shader::startUsing()
{
	//------------------------------------
	// Set the vertex and pixel shader stage state.

	//OMSetRenderTargets
	auto context = DXMain::getContext();

	context->IASetInputLayout(m_inputLayout.Get());

	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);

	//-----------
	// vertex shader

	// Lock the fog constant buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(m_vertexInfoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// Get a pointer to the data in the constant buffer.
	VertexInfoBuffer* dataPtr2 = (VertexInfoBuffer*)mappedResource.pData;
	// Copy the fog information into the fog constant buffer.
	memcpy(dataPtr2, &m_vertexInfoBufferData, sizeof(VertexInfoBuffer));
	// Unlock the constant buffer.
	context->Unmap(m_vertexInfoBuffer.Get(), 0);

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	//context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	ID3D11Buffer* bufs[2];
	bufs[0] = m_constantBuffer.Get();//.GetAddressOf()
	bufs[1] = m_vertexInfoBuffer.Get();
	//http://gamedev.stackexchange.com/questions/60668/how-to-use-updatesubresource-and-map-unmap#60672
	//DXMain::getContext()->UpdateSubresource(m_vertexInfoBuffer.Get(), 0, nullptr, &m_vertexInfoBufferData, 0, 0);
	//DXMain::getContext()->UpdateSubresource(m_vertexInfoBuffer2.Get(), 0, nullptr, &m_vertexInfoBufferData2, 0, 0);

	context->VSSetConstantBuffers(0, 2, bufs);

	//-----------
	// pixel shader

	// Lock the fog constant buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	context->Map(m_pixelInfoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	// Get a pointer to the data in the constant buffer.
	PixelInfoBuffer* dataPtr3 = (PixelInfoBuffer*)mappedResource2.pData;
	// Copy the fog information into the constant buffer.
	memcpy(dataPtr3, &m_pixelInfoBufferData, sizeof(PixelInfoBuffer));
	//dataPtr3->blurLevelAndUsesLight = m_pixelInfoBufferData.blurLevelAndUsesLight;
	// Unlock the constant buffer.
	context->Unmap(m_pixelInfoBuffer.Get(), 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	ID3D11Buffer* bufs2[1];
	bufs2[0] = m_pixelInfoBuffer.Get();
	context->PSSetConstantBuffers(0, 1, bufs2);
}

//---------------------------------------------------------------------------------------------
/*
void Shader::setRenderBlurLevel(int blurLevel)
{
	Assert(blurLevel >= 0 && blurLevel < 10);
	//m_fogBufferData.blurLevel = blurLevel;
	//bool usesLight = (m_pixelInfoBufferData.blurLevelAndUsesLight >= 10);
	//m_pixelInfoBufferData.blurLevelAndUsesLight = (usesLight ? 10 : 0) + blurLevel;
	m_pixelInfoBufferData.blurLevel = blurLevel;
}
*/
//---------------------------------------------------------------------------------------------

Shader::~Shader()
{
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_pixelInfoBuffer.Reset();
	m_blendState.Reset();

	m_vertexInfoBuffer.Reset();
	m_pixelInfoBuffer.Reset();
}

//---------------------------------------------------------------------------------------------
