#ifndef Picking3D_h_INCLUDED
#define Picking3D_h_INCLUDED

#include <vector>

#include "../../include/CoreUtils.h"
#include "../../include/BasicVertex.h"

//---------------------------------------------------------------------------------------------

namespace Picking3D
{
	typedef u16 TypeIndex;
	typedef const void* IndicesAndVerticesIdentifier;

	void enable(bool yesNo);

	Float3 get3DPosFrom2D(
		const Int2& pos, const Int2& winSize, 
		const float4x4& projectionMat, const float4x4& worldMat, const float4x4& viewMat, 
		const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane,
		IndicesAndVerticesIdentifier givenIndicesAndVerticesOrNull);

	std::vector<Float3> getAll3DPosFrom2D(const Int2& pos, const Int2& winSize,
		const float4x4& projectionMat, const float4x4& worldMat, const float4x4& viewMat,
		const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane);// TODO buggued (get only one point per VBO max)

	void* addToPicking(const void* vbo, const Meshes3D::BasicVertex* vertices, int verticesSize, const TypeIndex* indices, int indicesSize, bool isQuadInsteadOfTriangle);
	void removeFromPicking(const void* vbo);
}

//---------------------------------------------------------------------------------------------

#endif //Picking3D_h_INCLUDED
