#ifndef BasicVertex_h_INCLUDED
#define BasicVertex_h_INCLUDED

#include "CoreUtils.h"
#include "BasicMath.h"

namespace Meshes3D
{
	//---------------------------------------------------------------------
	/**
	* \brief A 3D geometric vertex with its position, normal vector, texture coordinate, and texture color modifier.
	*/

	struct BasicVertex
	{
		inline void copyFrom(const float* vertices, const float* normals, const float* texCoord, const float* col)
		{
			this->copyVerticesFrom(vertices);
			this->copyNormalsFrom(normals);
			this->copyTexCoordFrom(texCoord);
			this->copyColorFrom(col);
		}
		inline void copyVerticesFrom(const float vertices[3]) { memcpy(&pos, vertices, sizeof(float3)); }
		inline void copyNormalsFrom(const float normals[3]) { memcpy(&norm, normals, sizeof(float3)); }
		inline void copyTexCoordFrom(const float texCoord[2]) { memcpy(&tex, texCoord, sizeof(float2)); }
		inline void copyColorFrom(const float col[4]) { memcpy(&color, col, sizeof(float4)); }

		float3 pos;  // position
		float3 norm; // surface normal vector
		float2 tex;  // texture coordinate
		float4 color; // texture color modifier
	};

	//---------------------------------------------------------------------
	/**
	* \brief A 3D geometric quadrilateral (composed of 4 BasicVertex).
	*/

	class BasicQuad
	{
	public:
		inline void copyFrom(const BasicQuad* bq)
		{
			memcpy(this, bq, sizeof(BasicQuad));
		}
		inline void copyFrom(const float vertices[12], const float normals[12], const float texCoord[8], const float color[16])
		{
			this->copyVerticesFrom(vertices);
			this->copyNormalsFrom(normals);
			this->copyTexCoordFrom(texCoord);
			this->copyColorFrom(color);
		}
		inline void copyVerticesFrom(const float vertices[12]) { for (int i = 0; i < 4; ++i) basicVertex[i].copyVerticesFrom(&vertices[i * 3]); }
		inline void copyVerticesFrom(const Float3& src1, const Float3& src2, const Float3& src3, const Float3& src4)
		{
			basicVertex[0].copyVerticesFrom((float*)&src1);
			basicVertex[1].copyVerticesFrom((float*)&src2);
			basicVertex[2].copyVerticesFrom((float*)&src3);
			basicVertex[3].copyVerticesFrom((float*)&src4);
		}
		inline void copyNormalsFrom(const float normals[12]) { for (int i = 0; i < 4; ++i) basicVertex[i].copyNormalsFrom(&normals[i * 3]); }
		inline void copyTexCoordFrom(const float texCoord[8]) { for (int i = 0; i < 4; ++i) basicVertex[i].copyTexCoordFrom(&texCoord[i * 2]); }
		inline void copyTexCoordFrom(float t1, float t2, float t3, float t4, float t5, float t6, float t7, float t8)
		{
			basicVertex[0].tex.x = t1;
			basicVertex[0].tex.y = t2;
			basicVertex[1].tex.x = t3;
			basicVertex[1].tex.y = t4;
			basicVertex[2].tex.x = t5;
			basicVertex[2].tex.y = t6;
			basicVertex[3].tex.x = t7;
			basicVertex[3].tex.y = t8;
		}
		inline void copyColorFrom(const float col[4]) { for (int i = 0; i < 4; ++i) basicVertex[i].copyColorFrom(col); }
		inline const BasicVertex& getVertex(int index) const { return basicVertex[index]; }
		BasicVertex basicVertex[4];
	};

	//---------------------------------------------------------------------
}

#endif //BasicVertex_h_INCLUDED
