#ifndef GroupOfVBO_h_INCLUDED
#define GroupOfVBO_h_INCLUDED

#include <vector>
#include <map>
#include <algorithm>

#include "CoreUtils.h"
#include "Utils.h"

#include "Material.h"
#include "GroupOfQuads.h"

// forward declare
class GroupsOfQuadsByTexture;
class CompressedGroupsOfQuadsByTexture;
class VBOGroup;

class TextureReal;

//---------------------------------------------------------------------
/**
* \brief A bunch of renderable stuff.
*/

class RenderGroup
{
public:
	RenderGroup(MemoryPool* memoryPoolOrNull, bool init = true);

	// create uncompressed group of quads. Needed only if you put init = false in the constructor
	void createEmptyUncompressed();

	void mergeWith(const RenderGroup& anotherRenderGroup);

	// set uncompressed group of quads
	Meshes3D::GroupOfQuads* getGroupOfQuadsFromMaterial(const Material& material);
	// important note: groupOfQuads will be owned and deleted by the RenderGroup
	void setGroupOfQuadsForMaterial(const Material& material, Meshes3D::GroupOfQuads* groupOfQuads);
	void addGroupOfQuadsForMaterial(const Material& material, Meshes3D::GroupOfQuads* groupOfQuads);

	// create compressed group of quads from uncompressed group of quads
	void createCompressedFromUncompressed();

	// create uncompressed group of quads from compressed group of quads
	void createUncompressedFromCompressed();

	// create vertex buffer objects ; return the number of VBO created
	void createVBOsFromOther(const RenderGroup& otherRenderGroup);

	// idem but delete uncompressed group of quads
	int finalize(bool addToPicking) { this->createVBOs(addToPicking); this->dropUncompressed(); return this->getNbVBOs(); }

	void removeFromPicking();

	void dropCompressed();
	void dropUncompressed();
	void dropVBOs();

	void draw(const Float3& cameraPositionForTransparency, bool isShadowed = false) const;
	
	bool isAvailableCompressed() const   { return m_compressedGroupsOfQuadsByTexture != NULL; }
	bool isAvailableUncompressed() const { return m_groupsOfQuadsByTexture != NULL; }
	bool isDrawable() const              { return m_vboGroup != NULL; }
	
	int getNbVBOs() const;
	int getVertexCountSoFar() const;

	~RenderGroup();

	static bool isUsedMaterial(MaterialId tex);

	void get3DPosFrom2D(const Int2& pos2d, std::vector<Float3>& outPos3d);

	const VBOGroup* getVBOGroup() const { return m_vboGroup; }

private:
	void createVBOs(bool addToPicking);

	void createVBOsFrom(const GroupsOfQuadsByTexture& groupsOfQuadsByTexture);

	GroupsOfQuadsByTexture* m_groupsOfQuadsByTexture;
	CompressedGroupsOfQuadsByTexture* m_compressedGroupsOfQuadsByTexture;
	VBOGroup* m_vboGroup;

	std::vector<MaterialId> m_listOfTextures;

	static std::map<MaterialId, int> s_counts;

	MemoryPool* m_memoryPool;
};

//---------------------------------------------------------------------

#endif //GroupOfVBO_h_INCLUDED
