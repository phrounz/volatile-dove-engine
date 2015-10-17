
#include "GroupOfQuadsByTexture.h"
#include "../include/VBOGroup.h"

#include "../include/RenderGroup.h"

//---------------------------------------------------------------------
//---------------------------------------------------------------------

std::map<MaterialId, int> RenderGroup::s_counts;

//---------------------------------------------------------------------
//---------------------------------------------------------------------

RenderGroup::RenderGroup(MemoryPool* memoryPoolOrNull, bool init): m_compressedGroupsOfQuadsByTexture(NULL), m_vboGroup(NULL), m_groupsOfQuadsByTexture(NULL), m_memoryPool(memoryPoolOrNull)
{
	if (init) this->createEmptyUncompressed();
}

//---------------------------------------------------------------------

void RenderGroup::createEmptyUncompressed()
{
	Assert(m_groupsOfQuadsByTexture == NULL);
	m_groupsOfQuadsByTexture = new GroupsOfQuadsByTexture;
}

//---------------------------------------------------------------------

void RenderGroup::mergeWith(const RenderGroup& anotherRenderGroup)
{
	if (m_memoryPool != NULL)
	{
		size_t nbChunks, sizeAllocated, nbAlloc;
		m_memoryPool->getInfos(&nbChunks, &sizeAllocated, &nbAlloc);
		outputln("RenderGroup::mergeWith: nbChunks: " << nbChunks << ";sizeAllocated: " << sizeAllocated << ";nbAlloc: " << nbAlloc);
	}
	
	m_groupsOfQuadsByTexture->mergeWith(*anotherRenderGroup.m_groupsOfQuadsByTexture, m_memoryPool);
}

//---------------------------------------------------------------------

Meshes3D::GroupOfQuads* RenderGroup::getGroupOfQuadsFromMaterial(const Material& material)
{
	return m_groupsOfQuadsByTexture->getGroupOfQuadsFromTexture(material.getMaterialId(), m_memoryPool);
}

void RenderGroup::setGroupOfQuadsForMaterial(const Material& material, Meshes3D::GroupOfQuads* groupOfQuads)
{
	m_groupsOfQuadsByTexture->set(material.getMaterialId(), groupOfQuads);
}

void RenderGroup::addGroupOfQuadsForMaterial(const Material& material, Meshes3D::GroupOfQuads* groupOfQuads)
{
	m_groupsOfQuadsByTexture->add(material.getMaterialId(), groupOfQuads);
}

//---------------------------------------------------------------------

void RenderGroup::createCompressedFromUncompressed()
{
	Assert(m_compressedGroupsOfQuadsByTexture == NULL);
	m_compressedGroupsOfQuadsByTexture = m_groupsOfQuadsByTexture->getCompressed();
}

//---------------------------------------------------------------------

void RenderGroup::createUncompressedFromCompressed()
{
	Assert(m_groupsOfQuadsByTexture == NULL);
	m_groupsOfQuadsByTexture = m_compressedGroupsOfQuadsByTexture->getUncompressed();
}

//---------------------------------------------------------------------

void RenderGroup::dropCompressed()
{
	delete m_compressedGroupsOfQuadsByTexture;
	m_compressedGroupsOfQuadsByTexture = NULL;
}

//---------------------------------------------------------------------

void RenderGroup::dropUncompressed()
{
	delete m_groupsOfQuadsByTexture;
	m_groupsOfQuadsByTexture = NULL;
}

//---------------------------------------------------------------------

void RenderGroup::dropVBOs()
{
	if (m_vboGroup != NULL)
	{
		delete m_vboGroup;
		m_vboGroup = NULL;
	}
	
	iterateVectorConst(m_listOfTextures, MaterialId)
	{
		s_counts[*it]--;
	}
}

//---------------------------------------------------------------------

void RenderGroup::createVBOs(bool addToPicking)
{
	if (m_memoryPool != NULL)
	{
		size_t nbChunks, sizeAllocated, nbAlloc;
		m_memoryPool->getInfos(&nbChunks, &sizeAllocated, &nbAlloc);
		//outputln("RenderGroup::createVBOs: nbChunks: " << nbChunks << ";sizeAllocated: " << sizeAllocated<<";nbAlloc: " << nbAlloc);
	}

	Assert(m_groupsOfQuadsByTexture != NULL);
	m_groupsOfQuadsByTexture->simplify();
	
	this->createVBOsFrom(*m_groupsOfQuadsByTexture);

	if (!addToPicking)
		m_vboGroup->removeFromPicking();
}

//---------------------------------------------------------------------

void RenderGroup::createVBOsFromOther(const RenderGroup& otherRenderGroup)
{
	this->createVBOsFrom(*otherRenderGroup.m_groupsOfQuadsByTexture);
}

//---------------------------------------------------------------------

void RenderGroup::createVBOsFrom(const GroupsOfQuadsByTexture& groupsOfQuadsByTexture)
{
	if (m_vboGroup == NULL) m_vboGroup = new VBOGroup;

	const GroupsOfQuadsByTexture::SubGroup& goqm = groupsOfQuadsByTexture.getData();
	iterateMapConst(goqm, MaterialId, std::vector<Meshes3D::GroupOfQuads*>)
	{
		MaterialId tex = (*it).first;
		std::map<MaterialId, int>::const_iterator it2 = s_counts.find(tex);
		if (it2 == s_counts.end()) s_counts[tex] = 0;
		s_counts[tex]++;
		m_listOfTextures.push_back(tex);

		const std::vector<Meshes3D::GroupOfQuads*>& vec = (*it).second;
		iterateVectorConst2(vec, Meshes3D::GroupOfQuads*)
		{
			if ((*it2)->getNbQuads() > 0) m_vboGroup->insert(tex, (*it2)->getNbQuads(), (*it2)->getData());
		}
	}

	//if (m_vbos.size() > 0) std::cout << "Added " << m_vbos.size() << " VBOWithTexture (" << m_verticesCount << " vertices)." << std::endl;
}

//---------------------------------------------------------------------

int RenderGroup::getNbVBOs() const
{
	return m_vboGroup->getNbVBOs();
}

//---------------------------------------------------------------------

void RenderGroup::draw(const Float3& cameraPositionForTransparency, bool isShadowed) const
{
	m_vboGroup->draw(cameraPositionForTransparency, isShadowed);
}

//---------------------------------------------------------------------

int RenderGroup::getVertexCountSoFar() const
{
	return m_vboGroup->getVertexCount();
}

//---------------------------------------------------------------------

RenderGroup::~RenderGroup()
{
	if (m_groupsOfQuadsByTexture != NULL) this->dropUncompressed();
	if (m_compressedGroupsOfQuadsByTexture != NULL) this->dropCompressed();
	if (m_vboGroup != NULL) this->dropVBOs();
}

//---------------------------------------------------------------------

bool RenderGroup::isUsedMaterial(MaterialId tex)
{
	std::map<MaterialId, int>::const_iterator it = s_counts.find(tex);
	if (it != s_counts.end()) return (*it).second > 0;
	return false;
}

//---------------------------------------------------------------------

void RenderGroup::get3DPosFrom2D(const Int2& pos2d, std::vector<Float3>& outPos3d)
{
	outPos3d.push_back(m_vboGroup->get3DPosFrom2D(pos2d));
}

void RenderGroup::removeFromPicking()
{
	Assert(m_vboGroup != NULL);
	m_vboGroup->removeFromPicking();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
