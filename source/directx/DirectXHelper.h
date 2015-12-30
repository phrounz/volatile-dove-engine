#pragma once

#include <Windows.h>
// Helper utilities to make DirectX APIs work with exceptions

#include "directx_inc.h"

#include <string>

#include "../EngineError.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace D2D1;


namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
		if (FAILED(hr))
		{
			LPWSTR pText = new wchar_t[10000];
			::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pText, 9999, NULL);
			OutputDebugStringW(pText);
			std::string fullMessage = Utils::convertWStringToString(pText);
			delete [] pText;
			throw EngineError(fullMessage);

			// Set a breakpoint on this line to catch DirectX API errors
			throw Platform::Exception::CreateException(hr);
		}
    }

	inline void ThrowIfFailed(HRESULT hr, std::wstring message)
	{
		if (FAILED(hr))
		{
			LPWSTR pText = new wchar_t[10000];
			::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pText, 9999, NULL);
			std::wstring str = std::wstring(L"DX::ThrowIfFailed: ") + message + L" - ";
			OutputDebugStringW(str.c_str());
			OutputDebugStringW(pText);

			std::string fullMessage = Utils::convertWStringToString(str + pText);
			delete [] pText;

			throw EngineError(fullMessage);

			// Set a breakpoint on this line to catch DirectX API errors
			throw Platform::Exception::CreateException(hr);
		}
	}
}

namespace DirectXHelper
{
	DXGI_SWAP_CHAIN_DESC1 getSwapChainDesc(DXGI_FORMAT format);
	
	DXGI_SAMPLE_DESC getSampleDescNo();// no multisampling

	CD3D11_TEXTURE2D_DESC getDepthStencilDesc(int width, int height, DXGI_SAMPLE_DESC sampleDesc);
	D3D11_DEPTH_STENCIL_VIEW_DESC getDepthStencilViewDesc(const CD3D11_TEXTURE2D_DESC& depthStencilDesc, bool useMultisampling);

	D3D11_DEPTH_STENCIL_DESC getDepthStencilDesc2();

	D3D11_TEXTURE2D_DESC getTexture2DDescForRenderTarget(int width, int height, DXGI_SAMPLE_DESC sampleDesc);

	D3D11_VIEWPORT getScreenViewport(float width, float height);

	D3D11_TEXTURE2D_DESC getTexture2DDescForShader(int width, int height, int nbMipMap, DXGI_SAMPLE_DESC sampleDesc);
	D3D11_SHADER_RESOURCE_VIEW_DESC getTexture2DForShaderViewDesc(const D3D11_TEXTURE2D_DESC& textureDesc);
	D3D11_SAMPLER_DESC getTextureSampler(bool transparency);

	D3D11_RASTERIZER_DESC getRasterizerDesc(bool useMultisampling);

}