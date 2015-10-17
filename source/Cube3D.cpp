
#include "../include/BasicMath.h"

#include "../include/Cube3D.h"

namespace Meshes3D
{
	namespace Cube3D
	{

		//---------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------
		/*
	unsigned short cubeIndices[] =
	{
	0, 1, 2,
	0, 2, 3,

	4, 5, 6,
	4, 6, 7,

	8, 9, 10,
	8, 10, 11,

	12, 13, 14,
	12, 14, 15,

	16, 17, 18,
	16, 18, 19,

	20, 21, 22,
	20, 22, 23
	};

	// Create vertex and index buffers that define a simple unit cube.

	// In the array below, which will be used to initialize the cube vertex buffers,
	// multiple vertices are used for each corner to allow different normal vectors and
	// texture coordinates to be defined for each face.
	BasicVertex cubeVertices[] =
	{
	{ float3(-0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 0.0f) }, // +Y (top face)
	{ float3( 0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), float2(1.0f, 0.0f) },
	{ float3( 0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), float2(1.0f, 1.0f) },
	{ float3(-0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), float2(0.0f, 1.0f) },

	{ float3(-0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 0.0f) }, // -Y (bottom face)
	{ float3( 0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), float2(1.0f, 0.0f) },
	{ float3( 0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), float2(1.0f, 1.0f) },
	{ float3(-0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), float2(0.0f, 1.0f) },

	{ float3(0.5f,  0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) }, // +X (right face)
	{ float3(0.5f,  0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) },
	{ float3(0.5f, -0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), float2(1.0f, 1.0f) },
	{ float3(0.5f, -0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 1.0f) },

	{ float3(-0.5f,  0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) }, // -X (left face)
	{ float3(-0.5f,  0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) },
	{ float3(-0.5f, -0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 1.0f) },
	{ float3(-0.5f, -0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 1.0f) },

	{ float3(-0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 0.0f) }, // +Z (front face)
	{ float3( 0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(1.0f, 0.0f) },
	{ float3( 0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(1.0f, 1.0f) },
	{ float3(-0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), float2(0.0f, 1.0f) },

	{ float3( 0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 0.0f) }, // -Z (back face)
	{ float3(-0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(1.0f, 0.0f) },
	{ float3(-0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(1.0f, 1.0f) },
	{ float3( 0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), float2(0.0f, 1.0f) },
	};
	*/
		//---------------------------------------------------------------------

		static const float normalsTop[] = {
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f };

		static const float normalsBottom[] = {
			0.f, -1.f, 0.f,
			0.f, -1.f, 0.f,
			0.f, -1.f, 0.f,
			0.f, -1.f, 0.f };

		static const float normalsFront[] = {
			0.f, 0.f, 1.f,
			0.f, 0.f, 1.f,
			0.f, 0.f, 1.f,
			0.f, 0.f, 1.f };

		static const float normalsBack[] = {
			0.f, 0.f, -1.f,
			0.f, 0.f, -1.f,
			0.f, 0.f, -1.f,
			0.f, 0.f, -1.f };

		static const float normalsLeft[] = {
			-1.f, 0.f, 0.f,
			-1.f, 0.f, 0.f,
			-1.f, 0.f, 0.f,
			-1.f, 0.f, 0.f };

		static const float normalsRight[] = {
			1.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			1.f, 0.f, 0.f };

		Float4 COLOR_WHITE(1.f, 1.f, 1.f, 1.f);

		//---------------------------------------------------------------------

		void getVerticesAndTexCoordCube(
			Float3* vertices, Float3* normals, Float2* texCoord, // must be size 24
			const Float3& parTexCoordBegin, const Float3& parTexCoordEnd,
			const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide,
			const Float3& size, bool randomTexCoord)
		{
			const float reverse = 1.f;

			//top
			vertices[0] = Float3(-size.x(), size.y() * reverse, -size.z());
			vertices[1] = Float3(-size.x(), size.y() * reverse, size.z());
			vertices[2] = Float3(size.x(), size.y() * reverse, size.z());
			vertices[3] = Float3(size.x(), size.y() * reverse, -size.z());

			Float3 normalsTop2(0.f, 1.f*reverse, 0.f);
			normals[0] = normals[1] = normals[2] = normals[3] = normalsTop2;

			texCoord[3] = Float2(parTexCoordBeginOtherSide.x(), parTexCoordEndOtherSide.z());
			texCoord[2] = Float2(parTexCoordBeginOtherSide.x(), parTexCoordBeginOtherSide.z());
			texCoord[1] = Float2(parTexCoordEndOtherSide.x(), parTexCoordBeginOtherSide.z());
			texCoord[0] = Float2(parTexCoordEndOtherSide.x(), parTexCoordEndOtherSide.z());

			//bottom
			vertices[4] = Float3(-size.x(), -size.y() * reverse, -size.z());
			vertices[5] = Float3(size.x(), -size.y() * reverse, -size.z());
			vertices[6] = Float3(size.x(), -size.y() * reverse, size.z());
			vertices[7] = Float3(-size.x(), -size.y() * reverse, size.z());

			Float3 normalsBottom2(0.f, -1.f*reverse, 0.f);
			normals[4] = normals[5] = normals[6] = normals[7] = normalsBottom2;

			texCoord[4] = Float2(parTexCoordBegin.x(), parTexCoordBegin.z());
			texCoord[5] = Float2(parTexCoordEnd.x(), parTexCoordBegin.z());
			texCoord[6] = Float2(parTexCoordEnd.x(), parTexCoordEnd.z());
			texCoord[7] = Float2(parTexCoordBegin.x(), parTexCoordEnd.z());

			//front
			vertices[8] = Float3(-size.x(), -size.y(), size.z() * reverse);
			vertices[9] = Float3(size.x(), -size.y(), size.z() * reverse);
			vertices[10] = Float3(size.x(), size.y(), size.z() * reverse);
			vertices[11] = Float3(-size.x(), size.y(), size.z() * reverse);

			Float3 normalsFront2(0.f, 0.f, 1.f*reverse);
			normals[8] = normals[9] = normals[10] = normals[11] = normalsFront2;

			texCoord[8] = Float2(parTexCoordBegin.x(), parTexCoordBegin.y());
			texCoord[9] = Float2(parTexCoordEnd.x(), parTexCoordBegin.y());
			texCoord[10] = Float2(parTexCoordEnd.x(), parTexCoordEnd.y());
			texCoord[11] = Float2(parTexCoordBegin.x(), parTexCoordEnd.y());

			//back
			vertices[12] = Float3(-size.x(), size.y(), -size.z() * reverse);
			vertices[13] = Float3(size.x(), size.y(), -size.z() * reverse);
			vertices[14] = Float3(size.x(), -size.y(), -size.z() * reverse);
			vertices[15] = Float3(-size.x(), -size.y(), -size.z() * reverse);

			Float3 normalsBack2(0.f, 0.f, -1.f*reverse);
			normals[12] = normals[13] = normals[14] = normals[15] = normalsBack2;

			texCoord[12] = Float2(1.f - parTexCoordEndOtherSide.x(), parTexCoordEndOtherSide.y());
			texCoord[13] = Float2(1.f - parTexCoordBeginOtherSide.x(), parTexCoordEndOtherSide.y());
			texCoord[14] = Float2(1.f - parTexCoordBeginOtherSide.x(), parTexCoordBeginOtherSide.y());
			texCoord[15] = Float2(1.f - parTexCoordEndOtherSide.x(), parTexCoordBeginOtherSide.y());

			//left
			vertices[16] = Float3(-size.x() * reverse, -size.y(), -size.z());
			vertices[17] = Float3(-size.x() * reverse, -size.y(), size.z());
			vertices[18] = Float3(-size.x() * reverse, size.y(), size.z());
			vertices[19] = Float3(-size.x() * reverse, size.y(), -size.z());

			Float3 normalsLeft2(-1.f*reverse, 0.f, 0.f);
			normals[16] = normals[17] = normals[18] = normals[19] = normalsLeft2;

			texCoord[16] = Float2(parTexCoordBegin.z(), parTexCoordBegin.y());
			texCoord[17] = Float2(parTexCoordEnd.z(), parTexCoordBegin.y());
			texCoord[18] = Float2(parTexCoordEnd.z(), parTexCoordEnd.y());
			texCoord[19] = Float2(parTexCoordBegin.z(), parTexCoordEnd.y());

			//right
			vertices[20] = Float3(size.x() * reverse, -size.y(), -size.z());
			vertices[21] = Float3(size.x() * reverse, size.y(), -size.z());
			vertices[22] = Float3(size.x() * reverse, size.y(), size.z());
			vertices[23] = Float3(size.x() * reverse, -size.y(), size.z());

			Float3 normalsRight2(1.f*reverse, 0.f, 0.f);
			normals[20] = normals[21] = normals[22] = normals[23] = normalsRight2;

			texCoord[20] = Float2(parTexCoordEndOtherSide.z(), parTexCoordBeginOtherSide.y());
			texCoord[21] = Float2(parTexCoordEndOtherSide.z(), parTexCoordEndOtherSide.y());
			texCoord[22] = Float2(parTexCoordBeginOtherSide.z(), parTexCoordEndOtherSide.y());
			texCoord[23] = Float2(parTexCoordBeginOtherSide.z(), parTexCoordBeginOtherSide.y());

			if (randomTexCoord)
			{
				Float2 rand(Utils::random(parTexCoordEnd.x()), Utils::random(parTexCoordEnd.z()));
				texCoord[0] += Float2(rand);
				texCoord[1] += Float2(rand);
				texCoord[2] += Float2(rand);
				texCoord[3] += Float2(rand);
				texCoord[4] += Float2(rand);
				texCoord[5] += Float2(rand);
				texCoord[6] += Float2(rand);
				texCoord[7] += Float2(rand);

				rand = Float2(Utils::random(parTexCoordEnd.x()), Utils::random(parTexCoordEnd.y()));

				texCoord[8] += Float2(rand);
				texCoord[9] += Float2(rand);
				texCoord[10] += Float2(rand);
				texCoord[11] += Float2(rand);
				texCoord[12] += Float2(rand);
				texCoord[13] += Float2(rand);
				texCoord[14] += Float2(rand);
				texCoord[15] += Float2(rand);

				rand = Float2(Utils::random(parTexCoordEnd.z()), Utils::random(parTexCoordEnd.y()));

				texCoord[16] += Float2(rand);
				texCoord[17] += Float2(rand);
				texCoord[18] += Float2(rand);
				texCoord[19] += Float2(rand);
				texCoord[20] += Float2(rand);
				texCoord[21] += Float2(rand);
				texCoord[22] += Float2(rand);
				texCoord[23] += Float2(rand);
			}
		}

		//---------------------------------------------------------------------

		void Cube3D::getVerticesAndTexCoordCubeFace(
			const FaceId faceId,
			BasicQuad* basicQuad,
			const Float3& parTexCoordBegin, const Float3& parTexCoordEnd,
			const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide,
			const Float3& size, const Float3& posCube, bool randomTexCoord) //note: "size" is half cube size
		{
			if (faceId == FACE_TOP)
			{
				float vertices[12] = {
					-size.x() + posCube.x(), size.y() + posCube.y(), -size.z() + posCube.z(),
					-size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z(),
					size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z(),
					size.x() + posCube.x(), size.y() + posCube.y(), -size.z() + posCube.z() };
				basicQuad->basicVertex[0].copyVerticesFrom(&vertices[0]);
				basicQuad->basicVertex[1].copyVerticesFrom(&vertices[3]);
				basicQuad->basicVertex[2].copyVerticesFrom(&vertices[6]);
				basicQuad->basicVertex[3].copyVerticesFrom(&vertices[9]);
				/*
						basicQuad->basicVertex[0].pos = float3(-size.x() + posCube.x(), size.y() + posCube.y(),-size.z() + posCube.z());
						basicQuad->basicVertex[1].pos = float3(-size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z());
						basicQuad->basicVertex[2].pos = float3( size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z());
						basicQuad->basicVertex[3].pos = float3( size.x() + posCube.x(), size.y() + posCube.y(),-size.z() + posCube.z());
						*/
				memcpy(&(basicQuad->basicVertex[0].norm), normalsTop, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[1].norm), normalsTop, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[2].norm), normalsTop, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[3].norm), normalsTop, 3 * sizeof(float));

				float texCoord[8] = {
					parTexCoordBeginOtherSide.x(), 1.f - parTexCoordEndOtherSide.z(),
					parTexCoordBeginOtherSide.x(), 1.f - parTexCoordBeginOtherSide.z(),
					parTexCoordEndOtherSide.x(), 1.f - parTexCoordBeginOtherSide.z(),
					parTexCoordEndOtherSide.x(), 1.f - parTexCoordEndOtherSide.z() };
				basicQuad->basicVertex[0].copyTexCoordFrom(&texCoord[0]);
				basicQuad->basicVertex[1].copyTexCoordFrom(&texCoord[2]);
				basicQuad->basicVertex[2].copyTexCoordFrom(&texCoord[4]);
				basicQuad->basicVertex[3].copyTexCoordFrom(&texCoord[6]);
				/*
						basicQuad->basicVertex[0].tex = float2(parTexCoordBeginOtherSide.x(), 1.f-parTexCoordEndOtherSide.z());
						basicQuad->basicVertex[1].tex = float2(parTexCoordBeginOtherSide.x(), 1.f-parTexCoordBeginOtherSide.z());
						basicQuad->basicVertex[2].tex = float2(parTexCoordEndOtherSide.x(),   1.f-parTexCoordBeginOtherSide.z());
						basicQuad->basicVertex[2].tex = float2(parTexCoordEndOtherSide.x(),   1.f-parTexCoordEndOtherSide.z());
						*/
			}
			else if (faceId == FACE_BOTTOM)
			{
				float vertices[12] = {
					-size.x() + posCube.x(), -size.y() + posCube.y(), -size.z() + posCube.z(),
					size.x() + posCube.x(), -size.y() + posCube.y(), -size.z() + posCube.z(),
					size.x() + posCube.x(), -size.y() + posCube.y(), size.z() + posCube.z(),
					-size.x() + posCube.x(), -size.y() + posCube.y(), size.z() + posCube.z() };
				basicQuad->basicVertex[0].copyVerticesFrom(&vertices[0]);
				basicQuad->basicVertex[1].copyVerticesFrom(&vertices[3]);
				basicQuad->basicVertex[2].copyVerticesFrom(&vertices[6]);
				basicQuad->basicVertex[3].copyVerticesFrom(&vertices[9]);

				memcpy(&(basicQuad->basicVertex[0].norm), normalsBottom, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[1].norm), normalsBottom, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[2].norm), normalsBottom, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[3].norm), normalsBottom, 3 * sizeof(float));

				float texCoord[8] = {
					parTexCoordBegin.x(), 1.f - parTexCoordBegin.z(),
					parTexCoordEnd.x(), 1.f - parTexCoordBegin.z(),
					parTexCoordEnd.x(), 1.f - parTexCoordEnd.z(),
					parTexCoordBegin.x(), 1.f - parTexCoordEnd.z() };
				basicQuad->basicVertex[0].copyTexCoordFrom(&texCoord[0]);
				basicQuad->basicVertex[1].copyTexCoordFrom(&texCoord[2]);
				basicQuad->basicVertex[2].copyTexCoordFrom(&texCoord[4]);
				basicQuad->basicVertex[3].copyTexCoordFrom(&texCoord[6]);
			}
			else if (faceId == FACE_FRONT)
			{
				float vertices[12] = {
					-size.x() + posCube.x(), -size.y() + posCube.y(), size.z() + posCube.z(),
					size.x() + posCube.x(), -size.y() + posCube.y(), size.z() + posCube.z(),
					size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z(),
					-size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z() };
				basicQuad->basicVertex[0].copyVerticesFrom(&vertices[0]);
				basicQuad->basicVertex[1].copyVerticesFrom(&vertices[3]);
				basicQuad->basicVertex[2].copyVerticesFrom(&vertices[6]);
				basicQuad->basicVertex[3].copyVerticesFrom(&vertices[9]);

				memcpy(&(basicQuad->basicVertex[0].norm), normalsFront, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[1].norm), normalsFront, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[2].norm), normalsFront, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[3].norm), normalsFront, 3 * sizeof(float));

				float texCoord[8] = {
					parTexCoordBegin.x(), 1.f - parTexCoordBegin.y(),
					parTexCoordEnd.x(), 1.f - parTexCoordBegin.y(),
					parTexCoordEnd.x(), 1.f - parTexCoordEnd.y(),
					parTexCoordBegin.x(), 1.f - parTexCoordEnd.y() };
				basicQuad->basicVertex[0].copyTexCoordFrom(&texCoord[0]);
				basicQuad->basicVertex[1].copyTexCoordFrom(&texCoord[2]);
				basicQuad->basicVertex[2].copyTexCoordFrom(&texCoord[4]);
				basicQuad->basicVertex[3].copyTexCoordFrom(&texCoord[6]);
			}
			else if (faceId == FACE_BACK)
			{
				float vertices[12] = {
					-size.x() + posCube.x(), size.y() + posCube.y(), -size.z() + posCube.z(),
					size.x() + posCube.x(), size.y() + posCube.y(), -size.z() + posCube.z(),
					size.x() + posCube.x(), -size.y() + posCube.y(), -size.z() + posCube.z(),
					-size.x() + posCube.x(), -size.y() + posCube.y(), -size.z() + posCube.z() };
				basicQuad->basicVertex[0].copyVerticesFrom(&vertices[0]);
				basicQuad->basicVertex[1].copyVerticesFrom(&vertices[3]);
				basicQuad->basicVertex[2].copyVerticesFrom(&vertices[6]);
				basicQuad->basicVertex[3].copyVerticesFrom(&vertices[9]);

				memcpy(&(basicQuad->basicVertex[0].norm), normalsBack, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[1].norm), normalsBack, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[2].norm), normalsBack, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[3].norm), normalsBack, 3 * sizeof(float));

				float texCoord[8] = {
					1.f - parTexCoordBeginOtherSide.x(), 1.f - parTexCoordEndOtherSide.y(),
					1.f - parTexCoordEndOtherSide.x(), 1.f - parTexCoordEndOtherSide.y(),
					1.f - parTexCoordEndOtherSide.x(), 1.f - parTexCoordBeginOtherSide.y(),
					1.f - parTexCoordBeginOtherSide.x(), 1.f - parTexCoordBeginOtherSide.y() };
				basicQuad->basicVertex[0].copyTexCoordFrom(&texCoord[0]);
				basicQuad->basicVertex[1].copyTexCoordFrom(&texCoord[2]);
				basicQuad->basicVertex[2].copyTexCoordFrom(&texCoord[4]);
				basicQuad->basicVertex[3].copyTexCoordFrom(&texCoord[6]);
			}
			else if (faceId == FACE_LEFT)
			{
				float vertices[12] = {
					-size.x() + posCube.x(), -size.y() + posCube.y(), -size.z() + posCube.z(),
					-size.x() + posCube.x(), -size.y() + posCube.y(), size.z() + posCube.z(),
					-size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z(),
					-size.x() + posCube.x(), size.y() + posCube.y(), -size.z() + posCube.z() };
				basicQuad->basicVertex[0].copyVerticesFrom(&vertices[0]);
				basicQuad->basicVertex[1].copyVerticesFrom(&vertices[3]);
				basicQuad->basicVertex[2].copyVerticesFrom(&vertices[6]);
				basicQuad->basicVertex[3].copyVerticesFrom(&vertices[9]);

				memcpy(&(basicQuad->basicVertex[0].norm), normalsLeft, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[1].norm), normalsLeft, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[2].norm), normalsLeft, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[3].norm), normalsLeft, 3 * sizeof(float));

				float texCoord[8] = {
					parTexCoordBegin.z(), 1.f - parTexCoordBegin.y(),
					parTexCoordEnd.z(), 1.f - parTexCoordBegin.y(),
					parTexCoordEnd.z(), 1.f - parTexCoordEnd.y(),
					parTexCoordBegin.z(), 1.f - parTexCoordEnd.y() };
				basicQuad->basicVertex[0].copyTexCoordFrom(&texCoord[0]);
				basicQuad->basicVertex[1].copyTexCoordFrom(&texCoord[2]);
				basicQuad->basicVertex[2].copyTexCoordFrom(&texCoord[4]);
				basicQuad->basicVertex[3].copyTexCoordFrom(&texCoord[6]);
			}
			else if (faceId == FACE_RIGHT)
			{
				float vertices[12] = {
					size.x() + posCube.x(), -size.y() + posCube.y(), -size.z() + posCube.z(),
					size.x() + posCube.x(), size.y() + posCube.y(), -size.z() + posCube.z(),
					size.x() + posCube.x(), size.y() + posCube.y(), size.z() + posCube.z(),
					size.x() + posCube.x(), -size.y() + posCube.y(), size.z() + posCube.z() };
				basicQuad->basicVertex[0].copyVerticesFrom(&vertices[0]);
				basicQuad->basicVertex[1].copyVerticesFrom(&vertices[3]);
				basicQuad->basicVertex[2].copyVerticesFrom(&vertices[6]);
				basicQuad->basicVertex[3].copyVerticesFrom(&vertices[9]);

				memcpy(&(basicQuad->basicVertex[0].norm), normalsRight, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[1].norm), normalsRight, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[2].norm), normalsRight, 3 * sizeof(float));
				memcpy(&(basicQuad->basicVertex[3].norm), normalsRight, 3 * sizeof(float));

				float texCoord[8] = {
					parTexCoordEndOtherSide.z(), 1.f - parTexCoordBeginOtherSide.y(),
					parTexCoordEndOtherSide.z(), 1.f - parTexCoordEndOtherSide.y(),
					parTexCoordBeginOtherSide.z(), 1.f - parTexCoordEndOtherSide.y(),
					parTexCoordBeginOtherSide.z(), 1.f - parTexCoordBeginOtherSide.y() };
				basicQuad->basicVertex[0].copyTexCoordFrom(&texCoord[0]);
				basicQuad->basicVertex[1].copyTexCoordFrom(&texCoord[2]);
				basicQuad->basicVertex[2].copyTexCoordFrom(&texCoord[4]);
				basicQuad->basicVertex[3].copyTexCoordFrom(&texCoord[6]);
			}
			else
			{
				AssertRelease(false);
			}

			/*basicQuad->basicVertex[0].copyColorFrom(COLOR_WHITE.data);
			basicQuad->basicVertex[1].copyColorFrom(COLOR_WHITE.data);
			basicQuad->basicVertex[2].copyColorFrom(COLOR_WHITE.data);
			basicQuad->basicVertex[3].copyColorFrom(COLOR_WHITE.data);*/
		}

		//---------------------------------------------------------------------

	} //namespace Cube3D

}  //namespace Meshes3D
