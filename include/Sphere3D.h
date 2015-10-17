#ifndef Sphere3D_h_INCLUDED
#define Sphere3D_h_INCLUDED

#include <vector>

#include "CoreUtils.h"
#include "BasicVertex.h"

namespace Meshes3D
{
	//---------------------------------------------------------------------

	namespace Sphere3D
	{
		// TODO fix texture coordinates
		BasicVertex* getBasicVertices(int iFactor, float radius, int* outNbVertices);
	}

	//---------------------------------------------------------------------
}

#endif //Sphere3D_h_INCLUDED
