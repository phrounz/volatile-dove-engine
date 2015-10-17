
#include "DirectXHelper.h"

namespace
{
#ifdef USES_WINDOWS8_METRO
	const bool ENABLE_DOUBLE_BUFFERING = true;//note: this is compulsory for windows store apps
#else
	const bool ENABLE_DOUBLE_BUFFERING = false;
#endif
}


namespace DirectXHelper
{
	DXGI_SWAP_CHAIN_DESC1 getSwapChainDesc(DXGI_FORMAT format)
	{
		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 0;                                     // use automatic sizing //GetClientRect(m_window->m_hwnd, &rc);
		swapChainDesc.Height = 0;
		swapChainDesc.Format = format;           // this is the most common swapchain format
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc = sampleDesc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// CommonDirectXOptions::usingMultisampling() ? D3D11_USAGE_DEFAULT : DXGI_USAGE_RENDER_TARGET_OUTPUT;
		if (ENABLE_DOUBLE_BUFFERING)
		{
			swapChainDesc.BufferCount = 2;                               // use double buffering to enable flip
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // we recommend using this swap effect for all applications
		}
		else
		{
			swapChainDesc.BufferCount = 1;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
		}

		//swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		swapChainDesc.Flags = 0;

		return swapChainDesc;
	}


	DXGI_SAMPLE_DESC getSampleDescNo()
	{
		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;
		return sampleDesc;
	}


	CD3D11_TEXTURE2D_DESC getDepthStencilDesc(int width, int height, DXGI_SAMPLE_DESC sampleDesc)
	{
		// To allocate a 2-D surface as the depth/stencil buffer.
		// Create a descriptor for the depth/stencil buffer.
		//CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, backBufferDesc.Width,backBufferDesc.Height,1,1,D3D11_BIND_DEPTH_STENCIL);
		CD3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc = sampleDesc;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		/*CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			static_cast<UINT>(directXBase->m_renderTargetSize.Width),
			static_cast<UINT>(directXBase->m_renderTargetSize.Height),
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
			);
			// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(widthMulti),
		static_cast<UINT>(heightMulti),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL,
		D3D11_USAGE_DEFAULT,
		0,
		m_sampleSize,
		m_qualityFlags
		);

			*/

		return depthStencilDesc;
	}


	D3D11_DEPTH_STENCIL_DESC getDepthStencilDesc2()
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		// Depth test parameters
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		// Stencil test parameters
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;
		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		return dsDesc;
	}


	D3D11_DEPTH_STENCIL_VIEW_DESC getDepthStencilViewDesc(const CD3D11_TEXTURE2D_DESC& depthStencilDesc, bool useMultisampling)
	{
		//if (useMultisampling) return CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2DMS);
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = useMultisampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		return depthStencilViewDesc;
		//CreateDepthStencilView( ... &CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D),
	}


	D3D11_TEXTURE2D_DESC getTexture2DDescForRenderTarget(int width, int height, DXGI_SAMPLE_DESC sampleDesc)
	{
		D3D11_TEXTURE2D_DESC offScreenSurfaceDesc;
		ZeroMemory(&offScreenSurfaceDesc, sizeof(D3D11_TEXTURE2D_DESC));

		offScreenSurfaceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		offScreenSurfaceDesc.Width = static_cast<UINT>(width);
		offScreenSurfaceDesc.Height = static_cast<UINT>(height);
		offScreenSurfaceDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		offScreenSurfaceDesc.MipLevels = 1;
		offScreenSurfaceDesc.ArraySize = 1;
		offScreenSurfaceDesc.SampleDesc = sampleDesc;

		return offScreenSurfaceDesc;
	}


	D3D11_VIEWPORT getScreenViewport(float width, float height)
	{
		// Create a viewport descriptor of the full window size.
		return CD3D11_VIEWPORT(0.0f, 0.0f, width, height);
		/*m_screenViewport.TopLeftX = 0.0f;
		m_screenViewport.TopLeftY = 0.0f;
		m_screenViewport.Width = static_cast<float>(width);
		m_screenViewport.Height = static_cast<float>height);
		m_screenViewport.MinDepth = D3D11_MIN_DEPTH;
		m_screenViewport.MaxDepth = D3D11_MAX_DEPTH;*/
	}


	D3D11_TEXTURE2D_DESC getTexture2DDescForShader(int width, int height, int nbMipMap, DXGI_SAMPLE_DESC sampleDesc)
	{
		bool texturesAreMultisampled = (sampleDesc.Count > 1);

		// Create a texture description from information known a priori about the data.
		// Generalized texture loading code can be found in the Resource Loading sample.

		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Usage = texturesAreMultisampled ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		textureDesc.CPUAccessFlags = texturesAreMultisampled ? D3D11_CPU_ACCESS_WRITE : 0;
		//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;//0;

		// Most textures contain more than one MIP level.
		textureDesc.MipLevels = texturesAreMultisampled ? 1 : nbMipMap;

		// As this will not be a texture array, this parameter is ignored.
		textureDesc.ArraySize = 1;//3;

		// Multi-sampling.
		textureDesc.SampleDesc = sampleDesc;

		// Allow the texture to be bound as a shader resource.
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;// | D3D11_BIND_RENDER_TARGET

		return textureDesc;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC getTexture2DForShaderViewDesc(const D3D11_TEXTURE2D_DESC& textureDesc)
	{
		bool texturesAreMultisampled = (textureDesc.SampleDesc.Count > 1);

		D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
		ZeroMemory(&textureViewDesc, sizeof(textureViewDesc));
		textureViewDesc.Format = textureDesc.Format;
		textureViewDesc.ViewDimension = texturesAreMultisampled ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;//D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY
		textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
		textureViewDesc.Texture2D.MostDetailedMip = 0;//textureDesc.MipLevels - 1;// = 0;
		//textureViewDesc.Texture2D.MipLevels = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		//textureViewDesc.Texture2DArray.MipLevels = NB_MIPMAP;
		//textureViewDesc.Texture2DArray.MostDetailedMip = 0;
		//textureViewDesc.Texture2DArray.FirstArraySlice = 0;
		//textureViewDesc.Texture2DArray.ArraySize = NB_MIPMAP;//1
		return textureViewDesc;
	}


	D3D11_SAMPLER_DESC getTextureSampler(bool transparency)
	{
		// Once the texture view is created, create a sampler.  This defines how the color
		// for a particular texture coordinate is determined using the relevant texture data.
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,//D3D11_FILTER_ANISOTROPIC;//;
			// The sampler does not use anisotropic filtering, so this parameter is ignored.
			samplerDesc.MaxAnisotropy = 0;
		// Specify how texture coordinates outside of the range 0..1 are resolved.
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		// Use no special MIP clamping or bias.
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		// Don't use a comparison function.
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;// transparency ? D3D11_COMPARISON_NEVER : D3D11_COMPARISON_ALWAYS;//D3D11_COMPARISON_LESS;// 
		// Border address mode is not used, so this parameter is ignored.
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;

		return samplerDesc;
	}

	D3D11_RASTERIZER_DESC getRasterizerDesc(bool useMultisampling)
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;//10;
		rasterizerDesc.SlopeScaledDepthBias = 0.f;//10.f;
		rasterizerDesc.DepthBiasClamp = 0.f;//20.f;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = useMultisampling;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		return rasterizerDesc;
	}

}