
//---------------------------------------------------------------------

#include <wrl.h>
#include <d3d11_1.h>
#include <agile.h>
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;

//---------------------------------------------------------------------

#include "DXMain.h"
#include "Picking3D.h"
#include "DirectXHelper.h"
#include "../../include/BasicMath.h"
#include "../../include/Utils.h"
#include "../../include/AbstractMainClass.h"

//---------------------------------------------------------------------

#include "VBO.h"

struct VBOImpl
{
	VBOImpl(int indicesSize, int verticesSize)
		: m_indicesSize(indicesSize), m_verticesSize(verticesSize)
	{}
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	int m_indicesSize;
	int m_verticesSize;
};

//---------------------------------------------------------------------------------------------

void VBO::checkAndInit()
{
	// nothing to do
}

//---------------------------------------------------------------------------------------------

VBO::VBO(const Meshes3D::BasicVertex* basicVertices, int verticesSize, bool renderAsQuadsInsteadOfTriangles)
: vboImpl(new VBOImpl(-1, verticesSize)), m_renderAsQuadsInsteadOfTriangles(renderAsQuadsInsteadOfTriangles), m_addedToPicking(NULL)
{
	Assert(renderAsQuadsInsteadOfTriangles ? verticesSize % 4 == 0 : verticesSize % 3 == 0);
	m_addedToPicking = Picking3D::addToPicking((const void*)this, basicVertices, verticesSize, NULL, -1, m_renderAsQuadsInsteadOfTriangles);
	//Assert(!renderAsQuadsInsteadOfTriangles);// not implemented
	this->commonConstructor(basicVertices, verticesSize, NULL, -1);
}

//---------------------------------------------------------------------------------------------

VBO::VBO(const Meshes3D::BasicVertex* basicVertices, int verticesSize, const TypeIndex* indices, int indicesSize)
: vboImpl(new VBOImpl(indicesSize, verticesSize)), m_renderAsQuadsInsteadOfTriangles(false), m_addedToPicking(NULL)
{
	m_addedToPicking = Picking3D::addToPicking((const void*)this, basicVertices, verticesSize, indices, indicesSize, m_renderAsQuadsInsteadOfTriangles);
	this->commonConstructor(basicVertices, verticesSize, indices, indicesSize);
}

//---------------------------------------------------------------------------------------------

void VBO::commonConstructor(const Meshes3D::BasicVertex* basicVertices, int verticesSize, const TypeIndex* indices, int indicesSize)
{
	Assert(basicVertices != NULL);

    D3D11_BUFFER_DESC vertexBufferDesc = {0};
	vertexBufferDesc.ByteWidth = sizeof(Meshes3D::BasicVertex) * verticesSize;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = sizeof(Meshes3D::BasicVertex);

    D3D11_SUBRESOURCE_DATA vertexBufferData;
    vertexBufferData.pSysMem = basicVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
		DXMain::getDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &(vboImpl->m_vertexBuffer)
            )
        );// TODO: crash here
	
	if (indicesSize != -1)
	{
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.ByteWidth = sizeof(TypeIndex)* indicesSize;
		indexBufferDesc.StructureByteStride = sizeof(TypeIndex);
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		DX::ThrowIfFailed(
			DXMain::getDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&(vboImpl->m_indexBuffer)
				)
			);
	}
}

//---------------------------------------------------------------------

//int VBO::getVertexCount() {return vboImpl->m_verticesSize;}

//---------------------------------------------------------------------

void VBO::draw() const
{
    // Set the vertex and index buffers, and specify the way they define geometry.
	UINT stride = sizeof(Meshes3D::BasicVertex);
    UINT offset = 0;
    DXMain::getContext()->IASetVertexBuffers(
        0,
        1,
        vboImpl->m_vertexBuffer.GetAddressOf(),
        &stride,
        &offset
        );
	
	if (vboImpl->m_indicesSize != -1)
	{
		DXMain::getContext()->IASetIndexBuffer(
			vboImpl->m_indexBuffer.Get(),
			sizeof(TypeIndex) == sizeof(int32) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, // index format
			0                     // specify the base index in the buffer
			);

		// specify the way the vertex and index buffers define geometry
		DXMain::getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw.
		DXMain::getContext()->DrawIndexed(vboImpl->m_indicesSize, 0, 0);
	}
	else
	{
		// specify the way the vertex and index buffers define geometry
		DXMain::getContext()->IASetPrimitiveTopology(m_renderAsQuadsInsteadOfTriangles ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw.
		DXMain::getContext()->Draw(vboImpl->m_verticesSize, 0);
	
	}
	//DXMain::getContext()->Draw(vboImpl->m_verticesSize, 0);
}

//---------------------------------------------------------------------

VBO::~VBO()
{
	vboImpl->m_indexBuffer.Reset();
	vboImpl->m_vertexBuffer.Reset();
	delete vboImpl;
	this->removeFromPicking();
}

//---------------------------------------------------------------------

Float3 VBO::get3DPosFrom2D(const Int2& pos)
{
	Assert(m_addedToPicking != NULL);
	return Engine::instance().getScene3DMgr().get3DPosFrom2D(pos, m_addedToPicking);
}

void VBO::removeFromPicking()
{
	if (m_addedToPicking != NULL)
	{
		Picking3D::removeFromPicking((const void*)this);
		m_addedToPicking = NULL;
	}
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
