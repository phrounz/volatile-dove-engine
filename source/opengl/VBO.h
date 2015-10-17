#ifndef VBO_h_INCLUDED
#define VBO_h_INCLUDED

#include <map>
#include <set>
#include <vector>
#include "../../include/CoreUtils.h"

#include "../../include/BasicVertex.h"

struct VBOImpl;

//---------------------------------------------------------------------

class VBO
{
public:
	static void checkAndInit();

	typedef u16 TypeIndex;

	VBO(const Meshes3D::BasicVertex* vertices, int verticesSize, bool renderAsQuadsInsteadOfTriangles);
	VBO(const Meshes3D::BasicVertex* vertices, int verticesSize, const TypeIndex* indices, int indicesSize);
	void draw() const;
	~VBO();

private:
	VBOImpl* vboImpl;
	void commonConstructor(const Meshes3D::BasicVertex* vertices, int verticesSize, const TypeIndex* indices, int indicesSize);
};

//---------------------------------------------------------------------

#endif VBO_h_INCLUDED
