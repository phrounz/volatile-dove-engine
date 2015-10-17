
#include <algorithm>

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	#include "directx/VBO.h"
#else
	#include "opengl/VBO.h"
#endif

#include "TextureReal.h"

#include "../include/VBOGroup.h"

#include "../include/Engine.h"

namespace
{
	const int STEP_BY_VBO = 65536;
}

class ImprovedVBO
{
public:
	ImprovedVBO::ImprovedVBO(int nbVertices, const Meshes3D::BasicVertex* basicVertex, bool renderAsQuadsInsteadOfTriangles)
	{
		//Assert(size%4==0);
		float3 sumPos(0.f, 0.f, 0.f);
		int step = renderAsQuadsInsteadOfTriangles ? STEP_BY_VBO : STEP_BY_VBO - 1;

		for (int k = 0; k < nbVertices;)
		{
			int size = (nbVertices - k >= step ? step : nbVertices - k);

			VBO* vbo = NULL;

#if 1// defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

			if (renderAsQuadsInsteadOfTriangles)
			{
				int sizeIndex = size * 6 / 4;

				VBO::TypeIndex* indices = new VBO::TypeIndex[sizeIndex];// size should be less than max value of VBO::TypeIndex i.e. 65536

				for (int i = 0; i < size; i += 4)
				{
					int id = i * 6 / 4;
					indices[id] = i;
					indices[id + 1] = i + 2;
					indices[id + 2] = i + 1;
					indices[id + 3] = i;
					indices[id + 4] = i + 3;
					indices[id + 5] = i + 2;
					//Assert(k + i + 3 < nbVertices);
				}
				
				Assert(k + size - 1 < nbVertices);
				vbo = new VBO(&basicVertex[k], size, indices, sizeIndex);
				
				delete [] indices;
			}
			else
			{
				vbo = new VBO(&basicVertex[k], size, false);
			}
#else
			vbo = new VBO(&basicVertex[k], size, renderAsQuadsInsteadOfTriangles);
#endif
			m_vbos.push_back(vbo);

			k += step;
		}

		float sizeF = (float)nbVertices;
		m_globalPositionForTransparency = Float3(sumPos.x / sizeF, sumPos.y / sizeF, sumPos.z / sizeF);
		//outputln("ImprovedVBO::ImprovedVBO finished.");
	}

	//---------------------------------------------------------------------

	void ImprovedVBO::draw() const
	{
		iterateVectorConst(m_vbos, VBO*)
		{
			(*it)->draw();
		}
	}

	//---------------------------------------------------------------------

	void ImprovedVBO::computeDistanceToCamera(const Float3& cameraPosition) const
	{
		m_distanceToCamera = m_globalPositionForTransparency.distanceTo(cameraPosition);
	}

	//---------------------------------------------------------------------

	float ImprovedVBO::getDistanceToCamera() const { return m_distanceToCamera; }
	
	//---------------------------------------------------------------------

	ImprovedVBO::~ImprovedVBO()
	{
		iterateVector(m_vbos, VBO*)
		{
			delete (*it);
		}
	}

	//---------------------------------------------------------------------
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Float3 ImprovedVBO::get3DPosFrom2D(const Int2& pos2d)
	{
		bool defined = false;
		Float3 nearestPoint;
		float distanceNearestPoint = -1.f;
		Float3 cameraPosition = Engine::instance().getScene3DMgr().getCamera().getPosition();
		iterateVector(m_vbos, VBO*)
		{
			Float3 pos3d = (*it)->get3DPosFrom2D(pos2d);
			if (!defined)
			{
				nearestPoint = pos3d;
				distanceNearestPoint = pos3d.distanceTo(cameraPosition);
			}
			else
			{
				float dist = pos3d.distanceTo(cameraPosition);
				if (dist < distanceNearestPoint)
				{
					nearestPoint = pos3d;
					distanceNearestPoint = dist;
				}
			}
			
			defined = true;
		}
		return nearestPoint;
	}

	void ImprovedVBO::removeFromPicking()
	{
		iterateVector(m_vbos, VBO*)
		{
			(*it)->removeFromPicking();
		}
	}
#endif
	//---------------------------------------------------------------------

private:
	std::vector<VBO*> m_vbos;
	Float3 m_globalPositionForTransparency;
	mutable float m_distanceToCamera;

};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

class VBOWithTexture
{
public:

	VBOWithTexture(int nbVertices, const Meshes3D::BasicVertex* basicVertex, MaterialId tex)// render as triangles
		:m_tex(tex)
	{
		Assert(basicVertex != NULL && nbVertices > 0);
		m_improvedVBO = new ImprovedVBO(nbVertices, basicVertex, false);
	}
	VBOWithTexture(int nbQuads, const Meshes3D::BasicQuad* basicQuads, MaterialId tex)// render as quads
		:m_tex(tex)
	{
		Assert(basicQuads != NULL && nbQuads > 0);
		m_improvedVBO = new ImprovedVBO(nbQuads * 4, (const Meshes3D::BasicVertex*)basicQuads, true);
	}

	void draw(bool isShadowed) const
	{
		m_tex->setAsCurrentMaterial(isShadowed);
		m_improvedVBO->draw();
	}

	void computeDistanceToCamera(const Float3& cameraPosition) const { m_improvedVBO->computeDistanceToCamera(cameraPosition); }
	float getDistanceToCamera() const { return m_improvedVBO->getDistanceToCamera(); }

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Float3 get3DPosFrom2D(const Int2& pos) { return m_improvedVBO->get3DPosFrom2D(pos); }
	void removeFromPicking() { m_improvedVBO->removeFromPicking(); }
#endif

	~VBOWithTexture() { delete m_improvedVBO; }
private:
	ImprovedVBO* m_improvedVBO;
	MaterialId m_tex;
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------

namespace
{
	bool compareDistFarToNear(const VBOWithTexture* a, const VBOWithTexture* b)
	{
		return a->getDistanceToCamera() > b->getDistanceToCamera();
	}
}

//---------------------------------------------------------------------

void VBOGroup::draw(const Float3& cameraPositionForTransparency, bool isShadowed) const
{
	// draw vbos with standard textures
	iterateVectorConst(m_vbos, VBOWithTexture*)
	{
		(*it)->draw(isShadowed);
	}
	
	// draw vbos with transparent textures
	iterateVectorConst(m_vbosTransparent, VBOWithTexture*)
	{
		(*it)->computeDistanceToCamera(cameraPositionForTransparency);
	}
	std::sort(m_vbosTransparent.begin(), m_vbosTransparent.end(), &compareDistFarToNear);
	iterateVectorConst(m_vbosTransparent, VBOWithTexture*)
	{
		(*it)->draw(isShadowed);
	}
}

//---------------------------------------------------------------------

void VBOGroup::drawBunchOfVBOGroups(const Float3& cameraPositionForTransparency, const std::vector<const VBOGroup*>& vboGroups)
{
	iterateVectorConst(vboGroups, const VBOGroup*)
	{
		// draw vbos with standard textures
		const std::vector<VBOWithTexture*>& vbos = (*it)->m_vbos;
		iterateVectorConst2(vbos, VBOWithTexture*)
		{
			(*it2)->draw(false);
		}
	}

	std::vector<VBOWithTexture*> allTransparentVbos;
	// draw vbos with transparent textures
	iterateVectorConst(vboGroups, const VBOGroup*)
	{
		// push vbos with transparent textures
		const std::vector<VBOWithTexture*>& vbosTransparent = (*it)->m_vbosTransparent;
		iterateVectorConst2(vbosTransparent, VBOWithTexture*)
		{
			(*it2)->computeDistanceToCamera(cameraPositionForTransparency);
			allTransparentVbos.push_back(*it2);
		}
		
	}
	std::sort(allTransparentVbos.begin(), allTransparentVbos.end(), &compareDistFarToNear);
	iterateVectorConst(allTransparentVbos, VBOWithTexture*)
	{
		(*it)->draw(false);
	}
}

//---------------------------------------------------------------------

void VBOGroup::insert(MaterialId tex, int nbQuads, const Meshes3D::BasicQuad* basicQuads)
{
	Assert(basicQuads != NULL && nbQuads>0);
	VBOWithTexture* vbo = new VBOWithTexture(nbQuads, basicQuads, tex);
	Assert(vbo != NULL);
	(tex->containsTransparency() ? m_vbosTransparent:m_vbos).push_back(vbo);
	m_vertexCount += nbQuads*4;
	outputln("VBOGroup: added VBO from quads");
}

//---------------------------------------------------------------------

void VBOGroup::insert(MaterialId tex, int nbVertices, const Meshes3D::BasicVertex* basicVertices)
{
	Assert(basicVertices != NULL && nbVertices>0);
	VBOWithTexture* vbo = new VBOWithTexture(nbVertices, basicVertices, tex);
	Assert(vbo != NULL);
	(tex->containsTransparency() ? m_vbosTransparent:m_vbos).push_back(vbo);
	m_vertexCount += nbVertices;
	outputln("VBOGroup: added VBO from vertices");
}

//---------------------------------------------------------------------

VBOGroup::~VBOGroup()
{
	iterateVector(m_vbos, VBOWithTexture*)
	{
		delete (*it);
	}
	m_vbos.clear();
}


//---------------------------------------------------------------------
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

Float3 VBOGroup::get3DPosFrom2D(const Int2& pos2d)
{
	bool defined = false;
	Float3 nearestPoint;
	float distanceNearestPoint = -1.f;
	Float3 cameraPosition = Engine::instance().getScene3DMgr().getCamera().getPosition();
	iterateVector(m_vbos, VBOWithTexture*)
	{
		Float3 pos3d = (*it)->get3DPosFrom2D(pos2d);
		if (!defined)
		{
			nearestPoint = pos3d;
			distanceNearestPoint = pos3d.distanceTo(cameraPosition);
		}
		else
		{
			float dist = pos3d.distanceTo(cameraPosition);
			if (dist < distanceNearestPoint)
			{
				nearestPoint = pos3d;
				distanceNearestPoint = dist;
			}
		}

		defined = true;
	}
	return nearestPoint;
}

void VBOGroup::removeFromPicking()
{
	iterateVector(m_vbos, VBOWithTexture*)
	{
		(*it)->removeFromPicking();
	}
}

#endif

//---------------------------------------------------------------------
//---------------------------------------------------------------------
