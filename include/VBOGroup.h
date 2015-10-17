#ifndef VBOGroup_h_INCLUDED
#define VBOGroup_h_INCLUDED

#include <vector>

#include "CoreUtils.h"

#include "Material.h"

class ImprovedVBO;
struct BasicVertex;
class VBOWithTexture;

//---------------------------------------------------------------------
/**
* \brief A group of vertex buffer objects
*/

class VBOGroup
{
public:
	VBOGroup(): m_vertexCount(0) {}
	
	void insert(MaterialId tex, int nbQuads, const Meshes3D::BasicQuad* basicQuads);
	void insert(MaterialId tex, int nbVertices, const Meshes3D::BasicVertex* basicVertices);
	void draw(const Float3& cameraPositionForTransparency, bool isShadowed) const;

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	Float3 get3DPosFrom2D(const Int2& pos2d);
	void removeFromPicking();
#else
	Float3 get3DPosFrom2D(const Int2& pos2d) { return Float3(0.f, 0.f, 0.f); }
	void removeFromPicking() {}
#endif

	int getNbVBOs() const { return static_cast<int>(m_vbos.size()); }
	int getVertexCount() const { return m_vertexCount; }

	~VBOGroup();

	static void drawBunchOfVBOGroups(const Float3& cameraPositionForTransparency, const std::vector<const VBOGroup*>& vboGroups);
private:
	int m_vertexCount;
	std::vector<VBOWithTexture*> m_vbos;
	mutable std::vector<VBOWithTexture*> m_vbosTransparent;
};

//---------------------------------------------------------------------

#endif //VBOGroup_h_INCLUDED
