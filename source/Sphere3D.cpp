
#include <cmath>
#include "../include/Sphere3D.h"

namespace Meshes3D
{

	namespace Sphere3D
	{
		//---------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------

		// http://stackoverflow.com/questions/4405787/generating-vertices-for-a-sphere

		//D3DXCreateSphere()

		BasicVertex* getBasicVertices(int iFactor, float radius, int* outNbVertices)
		{
			int iPos = 0;
			*outNbVertices = (iFactor*iFactor * 6) + (iFactor * 6) + 6;
			BasicVertex* arrVertices = new BasicVertex[*outNbVertices];
			for (int i = 0; i < *outNbVertices; ++i) arrVertices[i].color = float4(1.f, 1.f, 1.f, 1.f);
			//int ui_ShapeCount = iFactor *iFactor * 2; // use when rendering

			float* arrV = new float[iFactor* iFactor * 3];

			for (int j = 0; j < iFactor; j++)
			{
				float theta = (static_cast<float>(M_PI)*j) / (iFactor);

				for (int i = 0; i < iFactor; i++)
				{
					iPos = j*iFactor + i;
					float phi = (2 * static_cast<float>(M_PI)*i) / (iFactor);
					arrV[iPos * 3 + 0] = (float)(sin(theta)*cos(phi))*radius;
					arrV[iPos * 3 + 1] = (float)(sin(theta)*sin(phi))*radius;
					arrV[iPos * 3 + 2] = (float)(cos(theta))*radius;
				}
			}

			int iNext = 0;

			for (int j = 0; j < iFactor; j++)
			{
				for (int i = 0; i < iFactor; i++)
				{
					if (i == iFactor - 1) iNext = 0;
					else iNext = i + 1;

					iPos = (j*iFactor * 6) + (i * 6);
					int index = (j*iFactor + i) * 3;
					arrVertices[iPos].pos = float3(arrV[index + 0], arrV[index + 1], arrV[index + 2]);
					int indexNext = (j*iFactor + iNext) * 3;
					arrVertices[iPos + 1].pos = float3(arrV[indexNext + 0], arrV[indexNext + 1], arrV[indexNext + 2]);


					if (j != iFactor - 1)
					{
						int index2 = (((j + 1)*iFactor) + i) * 3;
						arrVertices[iPos + 2].pos = float3(arrV[index2 + 0], arrV[index2 + 1], arrV[index2 + 2]);
					}
					else
						arrVertices[iPos + 2].pos = float3(0, 0, -1); //Create a pseudo triangle fan for the last set of triangles

					arrVertices[iPos + 0].norm = float3(arrVertices[iPos + 0].pos.x, arrVertices[iPos + 0].pos.y, arrVertices[iPos + 0].pos.z);
					arrVertices[iPos + 1].norm = float3(arrVertices[iPos + 1].pos.x, arrVertices[iPos + 1].pos.y, arrVertices[iPos + 1].pos.z);
					arrVertices[iPos + 2].norm = float3(arrVertices[iPos + 2].pos.x, arrVertices[iPos + 2].pos.y, arrVertices[iPos + 2].pos.z);

					arrVertices[iPos + 3].pos = float3(arrVertices[iPos + 2].pos.x, arrVertices[iPos + 2].pos.y, arrVertices[iPos + 2].pos.z);
					arrVertices[iPos + 4].pos = float3(arrVertices[iPos + 1].pos.x, arrVertices[iPos + 1].pos.y, arrVertices[iPos + 1].pos.z);

					if (j != iFactor - 1)
					{
						int index3 = (((j + 1)*iFactor) + iNext) * 3;
						arrVertices[iPos + 5].pos = float3(arrV[index3 + 0], arrV[index3 + 1], arrV[index3 + 2]);
					}
					else
						arrVertices[iPos + 5].pos = float3(0, 0, -1);

					arrVertices[iPos + 3].norm = float3(arrVertices[iPos + 3].pos.x, arrVertices[iPos + 3].pos.y, arrVertices[iPos + 3].pos.z);
					arrVertices[iPos + 4].norm = float3(arrVertices[iPos + 4].pos.x, arrVertices[iPos + 4].pos.y, arrVertices[iPos + 4].pos.z);
					arrVertices[iPos + 5].norm = float3(arrVertices[iPos + 5].pos.x, arrVertices[iPos + 5].pos.y, arrVertices[iPos + 5].pos.z);
				}
			}

			delete[] arrV;

			return arrVertices;
		}

		//---------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------
	}
}

/*GroupOfQuads* getGroupOfQuadsForSimpleSphere(float size)
{
const int VCOUNT = 20;
BasicVertex* basicVertices = Sphere3D::getBasicVertices(VCOUNT);
GroupOfQuads* groupOfQuads = new GroupOfQuads();
groupOfQuads->allocateMoreQuads(6);
float vertices[VCOUNT * 3];
float normals[VCOUNT * 3];
float texCoord[VCOUNT * 2];
for (int i = 0; i < VCOUNT; ++i)
{
vertices[i * 3 + 0] = basicVertices[i].pos.x;
vertices[i * 3 + 1] = basicVertices[i].pos.y;
vertices[i * 3 + 2] = basicVertices[i].pos.z;

normals[i * 3 + 0] = basicVertices[i].norm.x;
normals[i * 3 + 1] = basicVertices[i].norm.y;
normals[i * 3 + 2] = basicVertices[i].norm.z;

texCoord[i * 2 + 0] = basicVertices[i].tex.x;
texCoord[i * 2 + 1] = basicVertices[i].tex.y;
}
groupOfQuads->addQuad(vertices, normals, texCoord);//TODO

return groupOfQuads;
}*/