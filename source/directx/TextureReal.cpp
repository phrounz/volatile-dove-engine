
#include <wrl.h>
#include <d3d11_1.h>
#include <agile.h>

#include "../../include/AbstractMainClass.h"
#include "../../include/Utils.h"
#include "../../include/FileUtil.h"
#include "../../include/BasicMath.h"
#include "../../include/Image.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;

//#include "DirectXSample.h"
#include "DXMain.h"

#include "../TextureReal.h"

//-------------------------------------------------------------------------

const int NB_MIPMAP = 5;

//---------------------------------------------------------------------------------------------

struct TextureImpl
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
	bool m_containsTransparency;
};

//---------------------------------------------------------------------------------------------

TextureReal::TextureReal(const Image& image, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap)
:textureImpl(new TextureImpl()), m_textureFile("<not a file>")
{
	this->createTexture(image, colorFactorOrNull, clampToEdge, useMipmap);
}

//---------------------------------------------------------------------------------------------

TextureReal::TextureReal(const char* textureFile, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap)
:textureImpl(new TextureImpl()), m_textureFile(textureFile)
{
	//outputln("Creating texture from " << textureFile);
	// load image from disk
	Image* image = new Image(textureFile);
	this->createTexture(*image, colorFactorOrNull, clampToEdge, useMipmap);
	delete image;
}

//---------------------------------------------------------------------------------------------

void TextureReal::createTexture(const Image& image, const Float4* colorFactorOrNull, bool clampToEdge, bool useMipmap)
{
	Assert(useMipmap);

	Image* imageAltered = NULL;
	if (colorFactorOrNull)
	{
		imageAltered = new Image(image, *colorFactorOrNull);
	}

	textureImpl->m_containsTransparency = image.containsTransparency();

	//-------------------------------------------
	
	D3D11_SUBRESOURCE_DATA textureSubresourceData[NB_MIPMAP];// = {0};
	textureSubresourceData[0].pSysMem = image.getDataPixels();
    // Specify the size of a row in bytes, known a priori about the texture data.
	textureSubresourceData[0].SysMemPitch = image.size().width()*image.getBytesPerPixel();//1024;
    // As this is not a texture array or 3D texture, this parameter is ignored.
    textureSubresourceData[0].SysMemSlicePitch = 0;

	Image* imageDiv2 = new Image(image, 2);
	textureSubresourceData[1].pSysMem = imageDiv2->getDataPixels();
    textureSubresourceData[1].SysMemPitch = imageDiv2->size().width()*imageDiv2->getBytesPerPixel();
    textureSubresourceData[1].SysMemSlicePitch = 0;

	Image* imageDiv4 = new Image(image, 4);
	textureSubresourceData[2].pSysMem = imageDiv4->getDataPixels();
    textureSubresourceData[2].SysMemPitch = imageDiv4->size().width()*imageDiv4->getBytesPerPixel();
    textureSubresourceData[2].SysMemSlicePitch = 0;
	
	Image* imageDiv8 = new Image(image, 8);
	textureSubresourceData[3].pSysMem = imageDiv8->getDataPixels();
	textureSubresourceData[3].SysMemPitch = imageDiv8->size().width()*imageDiv8->getBytesPerPixel();
    textureSubresourceData[3].SysMemSlicePitch = 0;
	
	Image* imageDiv16 = new Image(image, 16);
	textureSubresourceData[4].pSysMem = imageDiv8->getDataPixels();
	textureSubresourceData[4].SysMemPitch = imageDiv8->size().width()*imageDiv8->getBytesPerPixel();
    textureSubresourceData[4].SysMemSlicePitch = 0;

	//-------------------------------------------
	// texture description

	D3D11_TEXTURE2D_DESC textureDesc = DirectXHelper::getTexture2DDescForShader(image.size().width(), image.size().height(), NB_MIPMAP, DirectXHelper::getSampleDescNo());

	const bool USING_MULTISAMPLED_TEXTURE = false;
	
    //Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateTexture2D(&textureDesc, USING_MULTISAMPLED_TEXTURE ? nullptr : textureSubresourceData, &textureImpl->m_texture)
        );

	if (USING_MULTISAMPLED_TEXTURE)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		mappedSubresource.RowPitch = image.size().width()*image.size().height()*image.getBytesPerPixel();
		mappedSubresource.DepthPitch = image.size().width()*image.getBytesPerPixel();

		// avoid "const" TO "not const" cast
		mappedSubresource.pData = NULL;// not used, actually
		const void* mappedSubresourcePData = image.getDataPixels();

		//DXMain::getContext()->Map(texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		//DXMain::getContext()->Unmap(texture.Get(), 0);
		DXMain::getContext()->UpdateSubresource(
			textureImpl->m_texture.Get(), 0, NULL, mappedSubresourcePData, mappedSubresource.RowPitch, mappedSubresource.DepthPitch);//CopyUpdateSubresource
	}

	//-------------------------------------------
	// texture view description

    // Once the texture is created, we must create a shader resource view of it
    // so that shaders may use it.  In general, the view description will match
    // the texture description.
	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc = DirectXHelper::getTexture2DForShaderViewDesc(textureDesc);
	
    DX::ThrowIfFailed(
		DXMain::getDevice()->CreateShaderResourceView(textureImpl->m_texture.Get(), &textureViewDesc, &(textureImpl->m_textureView))
        );

	//DXMain::getContext()->GenerateMips(textureImpl->m_textureView.Get());

	//-------------------------------------------
	// sampler description


	D3D11_SAMPLER_DESC samplerDesc = DirectXHelper::getTextureSampler(textureImpl->m_containsTransparency);
    
	DX::ThrowIfFailed(
		DXMain::getDevice()->CreateSamplerState(&samplerDesc, &(textureImpl->m_sampler))
        );
	
	//-------------------------------------------
	//"image" is now deleted

	if (colorFactorOrNull)
	{
		delete imageAltered;
	}
	delete imageDiv2;
	delete imageDiv4;
	delete imageDiv8;
	delete imageDiv16;
}

//---------------------------------------------------------------------------------------------

void TextureReal::setAsCurrentTexture() const
{	
	Engine::instance().getScene3DMgr().useUserDefinedTextureIdIfAvailable(FileUtil::basename(m_textureFile));

    DXMain::getContext()->PSSetShaderResources(0, 1, textureImpl->m_textureView.GetAddressOf());
	DXMain::getContext()->PSSetSamplers(0, 1, textureImpl->m_sampler.GetAddressOf());
}

//---------------------------------------------------------------------------------------------

bool TextureReal::containsTransparency() const
{
	return textureImpl->m_containsTransparency;
}

//---------------------------------------------------------------------------------------------

TextureReal::~TextureReal()
{
	textureImpl->m_textureView.Reset();
	textureImpl->m_sampler.Reset();
	textureImpl->m_texture.Reset();
	delete textureImpl;
}

//---------------------------------------------------------------------------------------------
