
#include "../include/CompressUtil.h"

#include "TextureReal.h"

#include "GroupOfQuadsByTexture.h"

//---------------------------------------------------------------------
//---------------------------------------------------------------------

namespace
{
	const bool SIMPLIFY_GROUP_OF_QUADS = false;
}

using namespace Meshes3D;

//---------------------------------------------------------------------
//---------------------------------------------------------------------

void CompressedGroupsOfQuadsByTexture::add(MaterialId tex, CompressUtil::CompressedBuffer* cb)
{
	m_data[tex].push_back(cb);
}

//---------------------------------------------------------------------

GroupsOfQuadsByTexture* CompressedGroupsOfQuadsByTexture::getUncompressed() const
{
	GroupsOfQuadsByTexture* groupsOfQuadsByTexture = new GroupsOfQuadsByTexture;

	iterateMapConst(m_data, MaterialId, std::vector<CompressUtil::CompressedBuffer*>)
	{
		iterateVectorConst2((*it).second, CompressUtil::CompressedBuffer*)
		{
			groupsOfQuadsByTexture->add((*it).first, new GroupOfQuads(*(*it2)));
		}
	}

	return groupsOfQuadsByTexture;
}

//---------------------------------------------------------------------

CompressedGroupsOfQuadsByTexture::~CompressedGroupsOfQuadsByTexture()
{
	for (std::map<MaterialId, std::vector<CompressUtil::CompressedBuffer*> >::iterator it = m_data.begin(); it != m_data.end(); it++)
	{
		std::vector<CompressUtil::CompressedBuffer*>& vgoq = (*it).second;
		iterateVector(vgoq, CompressUtil::CompressedBuffer*)
		{
			delete (*it)->dataCompressed;
			delete (*it);
		}
		vgoq.clear();
	}
	m_data.clear();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

void GroupsOfQuadsByTexture::simplify()
{
	if (SIMPLIFY_GROUP_OF_QUADS)
	{
		iterateMapConst(m_data, MaterialId, std::vector<GroupOfQuads*>)
		{
			iterateVectorConst2((*it).second, GroupOfQuads*)
			{
				(*it2)->simplify(false);
				(*it2)->simplify(true);
			}
		}
	}
}

//---------------------------------------------------------------------

void GroupsOfQuadsByTexture::mergeWith(const GroupsOfQuadsByTexture& anotherGroupOfQuadsByTexture, MemoryPool* memoryPool)
{
	const SubGroup& data = anotherGroupOfQuadsByTexture.m_data;
	iterateMapConst(data, MaterialId, std::vector<GroupOfQuads*>)
	{
		MaterialId materialId = (*it).first;
		std::vector<GroupOfQuads*>& goq = this->m_data[materialId];
		iterateVectorConst2((*it).second, GroupOfQuads*)
		{
			if (goq.size() == 0 || (*it).first->containsTransparency())
			{
				goq.push_back(new GroupOfQuads(memoryPool));
			}

			GroupOfQuads* groupOfQuads = goq[goq.size() - 1];
			groupOfQuads->mergeWith(**it2);
			if (groupOfQuads->getNbQuads() > 0)
			{
				Assert(groupOfQuads->getData() != NULL && groupOfQuads->getNbQuads()>0);
			}
		}
	}

	size_t nbChunks, sizeAllocated, nbAlloc;
	memoryPool->getInfos(&nbChunks, &sizeAllocated, &nbAlloc);
	outputln("GroupsOfQuadsByTexture::mergeWith: nbChunks: " << nbChunks <<";sizeAllocated: " << sizeAllocated<<";nbAlloc: " << nbAlloc);
}

//---------------------------------------------------------------------

GroupOfQuads* GroupsOfQuadsByTexture::getGroupOfQuadsFromTexture(MaterialId materialId, MemoryPool* memoryPool)
{
	GroupOfQuads* groupOfQuads = NULL;
	if (materialId->containsTransparency()) // materialId contains transparency -> add as chained
	{
		//always create a new one
		m_data[materialId].push_back(new GroupOfQuads(memoryPool));
		groupOfQuads = m_data[materialId][m_data[materialId].size() - 1];
	}
	else
	{
		// create only if it still does not exists
		if (m_data.find(materialId) == m_data.end())
			m_data[materialId].push_back(new GroupOfQuads(memoryPool));
		groupOfQuads = m_data[materialId][0];
	}
	return groupOfQuads;
}

//---------------------------------------------------------------------

void GroupsOfQuadsByTexture::set(MaterialId materialId, GroupOfQuads* groupOfQuads)
{
	if (m_data.find(materialId) != m_data.end())
	{
		this->deleteVect_(m_data[materialId]);
	}
	this->add(materialId, groupOfQuads);
}

//---------------------------------------------------------------------

void GroupsOfQuadsByTexture::add(MaterialId materialId, GroupOfQuads* groupOfQuads)
{
	m_data[materialId].push_back(groupOfQuads);
}

//---------------------------------------------------------------------

CompressedGroupsOfQuadsByTexture* GroupsOfQuadsByTexture::getCompressed() const
{
	CompressedGroupsOfQuadsByTexture* compressedGroupsOfQuadsByTexture = new CompressedGroupsOfQuadsByTexture;
	iterateMapConst(m_data, MaterialId, std::vector<GroupOfQuads*>)
	{
		iterateVectorConst2((*it).second, GroupOfQuads*)
		{
			if ((*it2)->getNbQuads() != 0)
				compressedGroupsOfQuadsByTexture->add((*it).first, new CompressUtil::CompressedBuffer((*it2)->getCompressedBuffer()));
		}
	}
	return compressedGroupsOfQuadsByTexture;
}

//---------------------------------------------------------------------

GroupsOfQuadsByTexture::~GroupsOfQuadsByTexture()
{
	outputln("Delete GroupsOfQuadsByTexture (" << m_data.size() <<")");
	for (SubGroup::iterator it = m_data.begin(); it != m_data.end(); it++)
	{
		this->deleteVect_((*it).second);
	}
	m_data.clear();
}

//---------------------------------------------------------------------

void GroupsOfQuadsByTexture::deleteVect_(std::vector<GroupOfQuads*>& vgoq)
{
	iterateVector(vgoq, GroupOfQuads*)
	{
		delete (*it);
	}
	vgoq.clear();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
