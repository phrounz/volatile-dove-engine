
#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>

#include <collection.h>

#include "Multisampling.h"

//#if defined(USES_WINDOWS8_DESKTOP)

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;

namespace
{
	const int SAMPLE_SIZE = 4;
}

bool Multisampling::checkSupport(Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice)
{
	UINT formatSupport = 0;
	HRESULT hr = d3dDevice->CheckFormatSupport(DXGI_FORMAT_B8G8R8A8_UNORM, &formatSupport);//DXGI_FORMAT_D24_UNORM_S8_UINT

	if ((formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) &&
		(formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET)
		)
	{
		return true;
	}
	return false;
}

DXGI_SAMPLE_DESC Multisampling::getSampleDescMultisampled(Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice)
{
	unsigned int numQualityLevels = 0;
	DX::ThrowIfFailed(
		d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 1, &numQualityLevels)
		);

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = SAMPLE_SIZE;
	sampleDesc.Quality = numQualityLevels - 1;
	return sampleDesc;
}

void Multisampling::setMultisampling(DXMain* directXBase)
{
	const float SCALING_FACTOR = 1.f;
	DXGI_SAMPLE_DESC sampleDesc = getSampleDescMultisampled(directXBase->m_d3dDevice);

	directXBase->m_offScreenSurface.Reset();
	directXBase->m_renderTargetView.Reset();
	directXBase->m_depthStencilView.Reset();
	directXBase->m_d2dTargetBitmap.Reset();

	//--------------------
	// CreateMultisampledRenderTarget

	int widthMulti = (int)directXBase->m_renderTargetSize.Width;
	int heightMulti = (int)directXBase->m_renderTargetSize.Height;

	D3D11_TEXTURE2D_DESC offScreenSurfaceDesc = DirectXHelper::getTexture2DDescForRenderTarget(widthMulti, heightMulti, sampleDesc);

	// Create a surface that's multisampled
	DX::ThrowIfFailed(
		directXBase->m_d3dDevice->CreateTexture2D(&offScreenSurfaceDesc, nullptr, &directXBase->m_offScreenSurface)
		);

	// Create a render target view 
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);
	DX::ThrowIfFailed(
		directXBase->m_d3dDevice->CreateRenderTargetView(directXBase->m_offScreenSurface.Get(), &renderTargetViewDesc, &directXBase->m_renderTargetView)
		);
	
	//--------------------
	// CreateMultisampledDepthBuffer

	// Create depth stencil buffer
	CD3D11_TEXTURE2D_DESC depthStencilDesc = DirectXHelper::getDepthStencilDesc(widthMulti, heightMulti, sampleDesc);
	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(directXBase->m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));

	// Create depth stencil view
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DMS);
	DX::ThrowIfFailed(directXBase->m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &directXBase->m_depthStencilView));

	//--------------------
	// SnippetCreateViewport

	// Set the 3D rendering viewport to target the entire window.
	directXBase->m_screenViewport = DirectXHelper::getScreenViewport(widthMulti / SCALING_FACTOR, heightMulti / SCALING_FACTOR);
	directXBase->m_d3dContext->RSSetViewports(1, &directXBase->m_screenViewport);
}

Multisampling::Multisampling(DXMain* directXBase)
{
	setMultisampling(directXBase);
}

void Multisampling::affectDxgiBuffer(DXMain* directXBase, Microsoft::WRL::ComPtr<IDXGISurface>& dxgiBuffer)
{
	DX::ThrowIfFailed(directXBase->m_offScreenSurface.As(&dxgiBuffer));
}

void Multisampling::resolve(DXMain* directXBase)
{
	unsigned int sub = D3D11CalcSubresource(0, 0, 1);

	directXBase->m_d3dContext->ResolveSubresource(
		directXBase->m_backBuffer.Get(),
		sub,
		directXBase->m_offScreenSurface.Get(),
		sub,
		DXGI_FORMAT_B8G8R8A8_UNORM
		);
}


//#endif //defined(USES_WINDOWS8_DESKTOP)
