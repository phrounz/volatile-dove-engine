
#include <map>
#include <DirectXMath.h>
#include <wrl\client.h>
#include <agile.h>
#include <DirectXCollision.h>
#include <math.h>

#include "../../include/Utils.h"
#include "../../include/Cube3D.h"

#include "../../include/GroupOfQuads.h"

#include "Picking3D.h"

// see tuto https://code.msdn.microsoft.com/How-to-pick-and-manipulate-089639ab

//#define ALTERNATIVE_CODE

namespace
{
	const bool DEBUG_VERBOSE = false;

	XMGLOBALCONST DirectX::XMVECTORF32 UNIT_VECTOR_EPSILON = { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f };
	inline bool checkXMVector3IsUnit(_In_ DirectX::FXMVECTOR V)
	{
		DirectX::XMVECTOR Difference = DirectX::XMVectorSubtract(DirectX::XMVector3Length(V), DirectX::XMVectorSplatOne());
		return DirectX::XMVector4Less(DirectX::XMVectorAbs(Difference), UNIT_VECTOR_EPSILON);
	}

	class IndicesAndVertices
	{
	public:

		IndicesAndVertices(const Meshes3D::BasicVertex* vertices2, int verticesSize2, const Picking3D::TypeIndex* indices2, int indicesSize2, bool isQuadInsteadOfTriangle2)
			:indicesSize(indicesSize2), verticesSize(verticesSize2), isQuadInsteadOfTriangle(isQuadInsteadOfTriangle2), vertices(NULL), indices(NULL), minPoint(0.f, 0.f, 0.f), maxPoint(0.f, 0.f, 0.f)
		{
			vertices = new Meshes3D::BasicVertex[verticesSize2];
			memcpy(vertices, vertices2, sizeof(Meshes3D::BasicVertex)*verticesSize2);
			if (indices2 != NULL)
			{
				indices = new Picking3D::TypeIndex[indicesSize2];
				memcpy(indices, indices2, sizeof(Picking3D::TypeIndex)*indicesSize2);
			}
			this->computeEmbracingCube();
		}

		~IndicesAndVertices()
		{
			if (indices != NULL) delete [] indices;
			delete [] vertices;

			delete m_embracingCubeGroupOfQuads;
			delete [] embracingCubeIndices;
		}

		void getPos(float3& pos0, float3& pos1, float3& pos2, int i, bool embracingCube) const
		{
			int vPerPt = this->getNbVerticesPerElement(embracingCube);

			if (embracingCube)
			{
				Assert(this->embracingCubeIndices != NULL);
				pos0 = this->embracingCubeVertices[this->embracingCubeIndices[i * vPerPt + 0]].pos;
				pos1 = this->embracingCubeVertices[this->embracingCubeIndices[i * vPerPt + 1]].pos;
				pos2 = this->embracingCubeVertices[this->embracingCubeIndices[i * vPerPt + 2]].pos;
			}
			else
			{
				
				if (this->indices != NULL)
				{
					Picking3D::TypeIndex i0 = this->indices[i * vPerPt + 0];
					Picking3D::TypeIndex i1 = this->indices[i * vPerPt + 1];
					Picking3D::TypeIndex i2 = this->indices[i * vPerPt + 2];
					pos0 = this->vertices[i0].pos;
					pos1 = this->vertices[i1].pos;
					pos2 = this->vertices[i2].pos;
				}
				else
				{
					pos0 = this->vertices[i * vPerPt + 0].pos;
					pos1 = this->vertices[i * vPerPt + 1].pos;
					pos2 = this->vertices[i * vPerPt + 2].pos;
				}
			}
		}

		UINT getNbIndices(bool embracingCube) const {
			if (embracingCube) return this->embracingCubeIndicesSize;
			return this->indices != NULL ? this->indicesSize : this->verticesSize;
		}
		
		int getNbVerticesPerElement(bool embracingCube) const {
			if (embracingCube) return 3;
			Assert(!this->isQuadInsteadOfTriangle);
			return this->isQuadInsteadOfTriangle ? 4 : 3;
		}

		bool isInEnglobingCube(const Float3& pos) const { return pos >= minPoint && pos < maxPoint; }

	private:

		bool isQuadInsteadOfTriangle;

		Meshes3D::BasicVertex* vertices;
		Picking3D::TypeIndex* indices;
		int verticesSize;
		int indicesSize;

		const Meshes3D::BasicVertex* embracingCubeVertices;
		Picking3D::TypeIndex* embracingCubeIndices;
		int embracingCubeVerticesSize;
		int embracingCubeIndicesSize;

		Meshes3D::GroupOfQuads* m_embracingCubeGroupOfQuads;

		Meshes3D::GroupOfQuads* getGroupOfQuadsForSimpleCube(const Float3& size, const Float3& position)
		{
			float4 color(1.f, 1.f, 1.f, 1.f);
			Meshes3D::GroupOfQuads* groupOfQuads = new Meshes3D::GroupOfQuads;
			groupOfQuads->allocateMoreQuads(6);
			for (int i = 0; i < 6; ++i)
			{
				Meshes3D::BasicQuad* basicQuad = groupOfQuads->getLast();
				Meshes3D::Cube3D::getVerticesAndTexCoordCubeFace(static_cast<FaceId>(i),
					basicQuad,
					Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f),
					Float3(0.f, 0.f, 0.f), Float3(1.f, 1.f, 1.f),
					size*0.5f, position, false);
				groupOfQuads->next();
			}
			return groupOfQuads;
		}

		void computeEmbracingCube()
		{
			UINT nb = this->getNbIndices(false);
			int vPerPt = this->getNbVerticesPerElement(false);

			for (UINT i = 0; i < nb / vPerPt; ++i)
			{
				float3 pos0;
				float3 pos1;
				float3 pos2;
				this->getPos(pos0, pos1, pos2, i, false);
				Float3 vertex3MinPoint = CoreUtils::minimum(CoreUtils::minimum(Float3(pos0.x, pos0.y, pos0.z), Float3(pos1.x, pos1.y, pos1.z)), Float3(pos2.x, pos2.y, pos2.z));
				Float3 vertex3MaxPoint = CoreUtils::maximum(CoreUtils::maximum(Float3(pos0.x, pos0.y, pos0.z), Float3(pos1.x, pos1.y, pos1.z)), Float3(pos2.x, pos2.y, pos2.z));
				if (i == 0)
				{
					minPoint = vertex3MinPoint;
					maxPoint = vertex3MaxPoint;
				}
				else
				{
					minPoint = CoreUtils::minimum(minPoint, vertex3MinPoint);
					maxPoint = CoreUtils::maximum(maxPoint, vertex3MaxPoint);
				}
			}

			m_embracingCubeGroupOfQuads = getGroupOfQuadsForSimpleCube(maxPoint - minPoint, (minPoint + maxPoint)*0.5f);
			
			embracingCubeVertices = (const Meshes3D::BasicVertex*)m_embracingCubeGroupOfQuads->getData();
			embracingCubeVerticesSize = (int)m_embracingCubeGroupOfQuads->getNbQuads() * 4;

			embracingCubeIndicesSize = embracingCubeVerticesSize * 6 / 4;
			embracingCubeIndices = new Picking3D::TypeIndex[embracingCubeIndicesSize];// size should be less than max value of VBO::TypeIndex i.e. 65536

			for (int i = 0; i < embracingCubeVerticesSize; i += 4)
			{
				int id = i * 6 / 4;
				embracingCubeIndices[id] = i;
				embracingCubeIndices[id + 1] = i + 2;
				embracingCubeIndices[id + 2] = i + 1;
				embracingCubeIndices[id + 3] = i;
				embracingCubeIndices[id + 4] = i + 3;
				embracingCubeIndices[id + 5] = i + 2;
			}
		}

		Float3 minPoint;
		Float3 maxPoint;
	};
}

namespace Picking3D
{
	std::map<const void*, IndicesAndVertices*> g_allObjects;
	bool g_enablePicking = false;

	void enable(bool yesNo)
	{
		if (g_enablePicking && !yesNo) // if disabling, clear all data
		{
			iterateMap(g_allObjects, const void*, IndicesAndVertices*)
			{
				delete (*it).second;
			}
			g_allObjects.clear();
		}
		g_enablePicking = yesNo;
	}

	DirectX::XMFLOAT4X4 xmFloat4x4FromFloat4x4(const float4x4& mat)
	{
		DirectX::XMFLOAT4X4 matrix;
		//memcpy(&matrix, &mat, sizeof(DirectX::XMFLOAT4X4));

		matrix._11 = mat._11;
		matrix._12 = mat._21;
		matrix._13 = mat._31;
		matrix._14 = mat._41;

		matrix._21 = mat._12;
		matrix._22 = mat._22;
		matrix._23 = mat._32;
		matrix._24 = mat._42;

		matrix._31 = mat._13;
		matrix._32 = mat._23;
		matrix._33 = mat._33;
		matrix._34 = mat._43;

		matrix._41 = mat._14;
		matrix._42 = mat._24;
		matrix._43 = mat._34;
		matrix._44 = mat._44;

		return matrix;
	}

#ifdef ALTERNATIVE_CODE

	void screenToView(
		_In_ float sx,
		_In_ float sy,
		_In_ const Int2& winSize,
		_In_ const DirectX::XMFLOAT4X4& projectionMatrix,
		_Outptr_ float * vx,
		_Outptr_ float * vy
		)
	{
		*vx = (2.0f * sx / winSize.width() - 1.0f) / projectionMatrix._11;
		*vy = (-2.0f * sy / winSize.height() + 1.0f) / projectionMatrix._22;
	}

	DirectX::XMVECTOR  vectorToLocal(DirectX::XMVECTOR inVec, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& modelMatrix)
	{
		DirectX::XMMATRIX viewMx = DirectX::XMLoadFloat4x4(&viewMatrix);
		DirectX::XMMATRIX modelMx = DirectX::XMLoadFloat4x4(&modelMatrix);
		DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&XMMatrixDeterminant(viewMx), viewMx);
		DirectX::XMMATRIX invModel = DirectX::XMMatrixInverse(&XMMatrixDeterminant(modelMx), modelMx);
		DirectX::XMMATRIX toLocal = invView * invModel;
		DirectX::XMFLOAT4 inVecF;
		DirectX::XMStoreFloat4(&inVecF, inVec);
		DirectX::XMVECTOR outVec = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		if(1.0f == inVecF.w)//point vector
		{
			outVec = XMVector3TransformCoord(inVec, toLocal);
		}
		else
		{
			outVec = DirectX::XMVector3TransformNormal(inVec, toLocal);
			outVec = DirectX::XMVector3Normalize(outVec);
		}
		return outVec;
	}

#endif

	void calculateVectorsPoint(
		float sx, float sy,
		const Int2& winSize,
		const float4x4& projectionMat, const float4x4& worldMat, const float4x4& viewMat,
		const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane, 
		DirectX::XMVECTOR* rayOrigin, DirectX::XMVECTOR* rayDir)
	{
		DirectX::XMFLOAT4X4 projectionMatrix = xmFloat4x4FromFloat4x4(projectionMat);
		DirectX::XMFLOAT4X4 worldMatrix = xmFloat4x4FromFloat4x4(worldMat);
		DirectX::XMFLOAT4X4 viewMatrix = xmFloat4x4FromFloat4x4(viewMat);
		
#ifdef ALTERNATIVE_CODE

		// The second way to get the ray direction and origin.
		float vx, vy;
		screenToView(sx, sy, winSize, projectionMatrix, &vx, &vy);
		//Get the ray
		*rayOrigin = vectorToLocal(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), viewMatrix, worldMatrix);
		*rayDir = vectorToLocal(DirectX::XMVectorSet(vx, vy, 1.0f, 0.0f), viewMatrix, worldMatrix);

#else

		DirectX::XMVECTOR vector1 = DirectX::XMVector3Unproject(
			DirectX::XMVectorSet(sx, sy, 0.f, 1.0f),
			0.0f,
			0.0f,
			(float)winSize.width(),
			(float)winSize.height(),
			0.f,//cameraNearClippingPlane, //0.f,
			1.f,//cameraFarClippingPlane, //1.f,
			DirectX::XMLoadFloat4x4(&projectionMatrix),
			DirectX::XMLoadFloat4x4(&viewMatrix),
			DirectX::XMLoadFloat4x4(&worldMatrix)
			);
		DirectX::XMVECTOR vector2 = DirectX::XMVector3Unproject(
			DirectX::XMVectorSet(sx, sy, 1.f, 1.0f),
			0.0f,
			0.0f,
			(float)winSize.width(),
			(float)winSize.height(),
			0.f,//cameraNearClippingPlane,
			1.f,//cameraFarClippingPlane,
			DirectX::XMLoadFloat4x4(&projectionMatrix),
			DirectX::XMLoadFloat4x4(&viewMatrix),
			DirectX::XMLoadFloat4x4(&worldMatrix)
			);

		*rayOrigin = vector1;
		*rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vector2, vector1));

#endif
	}

	bool intersectsVerticesSub(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDir, const IndicesAndVertices& indicesAndVertices, const Float3& cameraPosition, Float3* result, bool embracingCube)
	{
		UINT nb = indicesAndVertices.getNbIndices(embracingCube);
		int vPerPt = indicesAndVertices.getNbVerticesPerElement(embracingCube);

		Float3 nearestPoint(0, 0, 0);
		float distanceNearestPoint = -1.f;

		// Test inversects
		for (UINT i = 0; i < nb / vPerPt; ++i)
		{
			float3 pos0;
			float3 pos1;
			float3 pos2;
			indicesAndVertices.getPos(pos0, pos1, pos2, i, embracingCube);
			
			/*pos0 = float3(-10, -30, 30);
			pos1 = float3(-10, -30, 40);
			pos2 = float3(0, -30, 40);

			outputln("   " << (int)(void*)(&indicesAndVertices.vertices) << "," << i << ": " 
				<< pos0.x << "," << pos0.y << "," << pos0.z << " ; " << pos1.x << "," << pos1.y << "," << pos1.z << " ; " << pos2.x << "," << pos2.y << "," << pos2.z);*/

			DirectX::XMFLOAT3 p0 = DirectX::XMFLOAT3(pos0.x, pos0.y, pos0.z);
			DirectX::XMFLOAT3 p1 = DirectX::XMFLOAT3(pos1.x, pos1.y, pos1.z);
			DirectX::XMFLOAT3 p2 = DirectX::XMFLOAT3(pos2.x, pos2.y, pos2.z);

			DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&p0);// DirectX::XMVectorSet(p0.x, p0.y, p0.z, 0.0f);
			DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&p1);// DirectX::XMVectorSet((p1.x, p1.y, p1.z, 0.0f);
			DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&p2);// DirectX::XMVectorSet(p2.x, p2.y, p2.z, 0.0f);

			float t = 0.0f;
			// Set vector1 as ray origin, and (vector2 - vector1) as ray direction.
			// Note to normalize this direction vector.
			if (checkXMVector3IsUnit(rayDir) &&
				DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t) ||
				DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v2, v1, v0, t))
			{
				if (embracingCube)
				{
					if (DEBUG_VERBOSE) outputln("INTERSECTION " << embracingCube << " (" << pos0 << ";" << pos1 << ";" << pos2 << ")");
					return true;
				}
				DirectX::XMFLOAT3 originF;
				DirectX::XMFLOAT3 directionF;
				DirectX::XMStoreFloat3(&originF, rayOrigin);
				DirectX::XMStoreFloat3(&directionF, rayDir);

				Float3 intersectionPoint = Float3(originF.x, originF.y, originF.z) + Float3(directionF.x, directionF.y, directionF.z) * t;
				float distanceToCamera = intersectionPoint.distanceTo(cameraPosition);
				if (distanceToCamera < distanceNearestPoint || distanceNearestPoint == -1.f)
				{
					nearestPoint = intersectionPoint;
					distanceNearestPoint = distanceToCamera;
				}
				
				if (DEBUG_VERBOSE) outputln("INTERSECTION " << embracingCube << " (" << pos0 << ";" << pos1 << ";" << pos2 << ")"
					<< (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t)) << ", " << t << ", " << i << " / " << (nb / vPerPt)
					<< "    " << *result);
				//*output = Float3((pos0.x + pos1.x + pos2.x) / 3.f, (pos0.y + pos1.y + pos2.y) / 3.f, (pos0.z + pos1.z + pos2.z) / 3.f);
				//return true;
			}

		}

		*result = nearestPoint;
		return distanceNearestPoint != -1.f;
	}

	bool intersectsVertices(const DirectX::XMVECTOR& rayOrigin, const DirectX::XMVECTOR& rayDir, const IndicesAndVertices& indicesAndVertices, const Float3& cameraPosition, Float3* result)
	{
		return 
			(/*indicesAndVertices.isInEnglobingCube(cameraPosition) || */intersectsVerticesSub(rayOrigin, rayDir, indicesAndVertices, cameraPosition, result, true)) // intersects englobing cube
			&& intersectsVerticesSub(rayOrigin, rayDir, indicesAndVertices, cameraPosition, result, false); // intersects actual vertices
	}

	void* addToPicking(const void* vbo, const Meshes3D::BasicVertex* vertices, int verticesSize, const TypeIndex* indices, int indicesSize, bool isQuadInsteadOfTriangle)
	{
		if (g_enablePicking)
		{
			Assert(g_allObjects.find(vbo) == g_allObjects.end());
			g_allObjects[vbo] = new IndicesAndVertices(vertices, verticesSize, indices, indicesSize, isQuadInsteadOfTriangle);
			return g_allObjects[vbo];
		}
		return NULL;
	}

	void removeFromPicking(const void* vbo)
	{
		if (g_allObjects.find(vbo) != g_allObjects.end())
		{
			delete g_allObjects[vbo];
			g_allObjects.erase(vbo);
		}
	}

	Float3 get3DPosFrom2D(const Int2& pos, const Int2& winSize, 
		const float4x4& projectionMat, const float4x4& worldMat, const float4x4& viewMat, 
		const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane, 
		IndicesAndVerticesIdentifier givenIndicesAndVerticesOrNull)
	{
		if (!g_enablePicking) return Float3(0.f, 0.f, 0.f);
		DirectX::XMVECTOR rayOrigin;
		DirectX::XMVECTOR rayDir;
		calculateVectorsPoint((float)pos.x(), (float)pos.y(), winSize, projectionMat, worldMat, viewMat, cameraPosition, cameraNearClippingPlane, cameraFarClippingPlane, &rayOrigin, &rayDir);

		Float3 nearestPoint(0, cameraFarClippingPlane, 0);
		Float3 intersection;
		float distanceNearestPoint = -1.f;

		if (givenIndicesAndVerticesOrNull != NULL)
		{
			if (intersectsVertices(rayOrigin, rayDir, *reinterpret_cast<const IndicesAndVertices*>(givenIndicesAndVerticesOrNull), cameraPosition, &intersection))
			{
				nearestPoint = intersection;
				if (DEBUG_VERBOSE) outputln("Picking against " << givenIndicesAndVerticesOrNull << ": " << pos.x() << ", " << pos.y() << "->" << intersection);
			}
		}
		else
		{
			iterateMapConst(g_allObjects, const void*, IndicesAndVertices*)
			{
				if (intersectsVertices(rayOrigin, rayDir, *(*it).second, cameraPosition, &intersection))
				{
					float distance = intersection.distanceTo(cameraPosition);
					if (distance < distanceNearestPoint || distanceNearestPoint == -1.f)
					{
						nearestPoint = intersection;
						distanceNearestPoint = distance;
					}
					if (DEBUG_VERBOSE) outputln("Picking: " << pos.x() << "," << pos.y() << "->" << intersection);
				}
			}
		}

		//if (DEBUG_VERBOSE) outputln("Nearest:" << pos.x() << "," << pos.y() << "->" << nearestPoint);

		return nearestPoint;
	}

	std::vector<Float3> getAll3DPosFrom2D(const Int2& pos, const Int2& winSize,
		const float4x4& projectionMat, const float4x4& worldMat, const float4x4& viewMat,
		const Float3& cameraPosition, float cameraNearClippingPlane, float cameraFarClippingPlane)
	{
		DirectX::XMVECTOR rayOrigin;
		DirectX::XMVECTOR rayDir;
		calculateVectorsPoint((float)pos.x(), (float)pos.y(), winSize, projectionMat, worldMat, viewMat, cameraPosition, cameraNearClippingPlane, cameraFarClippingPlane, &rayOrigin, &rayDir);

		std::vector<Float3> results;
		Float3 intersection;
		iterateMapConst(g_allObjects, const void*, IndicesAndVertices*)
		{
			if (intersectsVertices(rayOrigin, rayDir, *(*it).second, cameraPosition, &intersection))
			{
				results.push_back(intersection);
			}
			//outputln("\n" << pos.x() << "," << pos.y() << "->" << intersection);
		}

		return results;
	}
}
