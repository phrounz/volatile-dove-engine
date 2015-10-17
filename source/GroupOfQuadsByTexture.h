#ifndef GroupOfQuadsByTexture_h_INCLUDED
#define GroupOfQuadsByTexture_h_INCLUDED

#include <map>
#include <vector>
#include <list>
#include <algorithm>

#include "../include/GroupOfQuads.h"
#include "../include/Material.h"

//typedef TextureReal TextureReal;

class GroupsOfQuadsByTexture;

//---------------------------------------------------------------------

class CompressedGroupsOfQuadsByTexture
{
public:
	CompressedGroupsOfQuadsByTexture() {}
	void add(MaterialId texture, CompressUtil::CompressedBuffer* cb);
	GroupsOfQuadsByTexture* getUncompressed() const;
	~CompressedGroupsOfQuadsByTexture();
private:
	std::map<MaterialId, std::vector<CompressUtil::CompressedBuffer*> > m_data;
};

//---------------------------------------------------------------------

class GroupsOfQuadsByTexture
{
public:
	typedef std::map<MaterialId, std::vector<Meshes3D::GroupOfQuads*> > SubGroup;

	GroupsOfQuadsByTexture() {}
	void simplify();
	void mergeWith(const GroupsOfQuadsByTexture& anotherGroupOfQuadsByTexture, MemoryPool* memoryPool);
	Meshes3D::GroupOfQuads* getGroupOfQuadsFromTexture(MaterialId texture, MemoryPool* memoryPool);
	void set(MaterialId texture, Meshes3D::GroupOfQuads* groupOfQuads);
	void add(MaterialId texture, Meshes3D::GroupOfQuads* groupOfQuads);
	const SubGroup& getData()    const { return m_data;}
	CompressedGroupsOfQuadsByTexture* getCompressed() const;
	~GroupsOfQuadsByTexture();
private:
	SubGroup m_data;
	void deleteVect_(std::vector<Meshes3D::GroupOfQuads*>& vgoq);
};

//---------------------------------------------------------------------

#endif //GroupOfQuadsByTexture_h_INCLUDED
