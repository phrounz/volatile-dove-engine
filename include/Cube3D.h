#ifndef Cube3D_h_INCLUDED
#define Cube3D_h_INCLUDED

#include "CoreUtils.h"
#include "BasicVertex.h"

namespace Meshes3D
{
	//---------------------------------------------------------------------
	namespace Cube3D
	{
		void getVerticesAndTexCoordCube(
			Float3* vertices, Float3* normals, Float2* texCoord,
			const Float3& parTexCoordBegin, const Float3& parTexCoordEnd,
			const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide,
			const Float3& size, bool randomTexCoord); // must be size 24

		enum FaceId { FACE_TOP = 0, FACE_BOTTOM = 1, FACE_FRONT = 2, FACE_BACK = 3, FACE_LEFT = 4, FACE_RIGHT = 5 };

		void getVerticesAndTexCoordCubeFace(
			const FaceId faceId,
			BasicQuad* basicQuad,
			const Float3& parTexCoordBegin, const Float3& parTexCoordEnd,
			const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide,
			const Float3& size, const Float3& posCube, bool randomTexCoord); // must be size 24
	}

	//---------------------------------------------------------------------
}

typedef Meshes3D::Cube3D::FaceId FaceId;

#endif //Cube3D_h_INCLUDED
