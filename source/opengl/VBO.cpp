
#include "ARB_Multisample.h"
#include "GLExtension.h"

#include "../../include/Utils.h"

#include "../../include/Engine.h"
#include "VBO.h"

namespace
{
	const bool DEBUG_VERBOSE = false;
}

class Fog
{
public:
	static bool isVolumetricFog() {return false;}
};

//#include "../Thread.h"

//---------------------------------------------------------------------
// vertex buffer objects
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=45

// VBO Extension Definitions from glext.h
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
//typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;					// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;					// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;					// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;				// VBO Deletion Procedure
PFNGLFOGCOORDPOINTERPROC glFogCoordPointer = NULL;

static bool isExtensionSupport = false;

//---------------------------------------------------------------------

struct VBOImpl
{
	VBOImpl():idIndices(-1) {}

	unsigned int idVertices;
	int vertexCount;

	unsigned int idFogCoords;
	unsigned int fogCoordsSize;

	int idIndices;
	unsigned int indicesSize;

	bool m_quadsInsteadOfTriangles;
};

//---------------------------------------------------------------------

void VBO::checkAndInit()
{
	isExtensionSupport = GLExtension::isExtensionSupported( "GL_ARB_vertex_buffer_object");
	if (isExtensionSupport)
	{
#ifndef USES_JS_EMSCRIPTEN
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
		glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC) wglGetProcAddress("glFogCoordPointer");
#endif
	}
	else
	{
		std::cout << "!!! ERROR !!! vertex buffer objects are not supported, nothing will be displayed." << std::endl;
	}
}

//---------------------------------------------------------------------

VBO::VBO(const Meshes3D::BasicVertex* vertices, int verticesSize, bool renderAsQuadsInsteadOfTriangles)
:vboImpl(new VBOImpl)
{
	vboImpl->m_quadsInsteadOfTriangles = renderAsQuadsInsteadOfTriangles;
	this->commonConstructor(vertices, verticesSize, NULL, -1);
}

//---------------------------------------------------------------------

VBO::VBO(const Meshes3D::BasicVertex* vertices, int verticesSize, const TypeIndex* indices, int indicesSize)
:vboImpl(new VBOImpl)
{
	vboImpl->m_quadsInsteadOfTriangles = false;
	Assert(indices != NULL);
	this->commonConstructor(vertices, verticesSize, indices, indicesSize);
}
/*
struct MyVertex
{
float x, y, z;        //Vertex
float nx, ny, nz;     //Normal
float s0, t0;         //Texcoord0
};
*/
//---------------------------------------------------------------------

void VBO::commonConstructor(const Meshes3D::BasicVertex* vertices, int verticesSize, const TypeIndex* indices, int indicesSize)
{
	outputln("Building VBO with " << verticesSize << " vertices and " << indicesSize << " indices");
	if (!isExtensionSupport) {outputln("      extension not supported");return;}

	if (DEBUG_VERBOSE) outputln("      extension supported");
	vboImpl->vertexCount = verticesSize;

	//Thread::claimSemaphore();

	//std::cout << "Adding VBO of " << vboImpl->vertexCount << " vertices." << std::endl;

	glGenBuffersARB( 1, &vboImpl->idVertices );
	if (DEBUG_VERBOSE) outputln("        Gen buffers ok.");
	glBindBufferARB( GL_ARRAY_BUFFER, vboImpl->idVertices );
	if (DEBUG_VERBOSE) outputln("        Bind buffer ok." << vboImpl->idVertices);
	glBufferDataARB( GL_ARRAY_BUFFER, vboImpl->vertexCount*sizeof(Meshes3D::BasicVertex), vertices, GL_STATIC_DRAW );
	if (DEBUG_VERBOSE) outputln("        Buffer data vertices done." << vboImpl->vertexCount << "," << sizeof(Meshes3D::BasicVertex));

	if (indices != NULL)
	{
		unsigned int idIndicesUnsigned;
		glGenBuffersARB( 1, &idIndicesUnsigned );
		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, idIndicesUnsigned );
		vboImpl->idIndices = static_cast<int>(idIndicesUnsigned);
		glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER, indicesSize*sizeof(TypeIndex), indices, GL_STATIC_DRAW );
		vboImpl->indicesSize = indicesSize;
		if (DEBUG_VERBOSE) outputln("        Indices done.");
	}

	//Thread::releaseSemaphore();

	//http://stackoverflow.com/questions/10545479/why-does-binding-a-gl-element-array-buffer-to-0-produce-a-memmove-error
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (Fog::isVolumetricFog())
	{
		vboImpl->fogCoordsSize = vboImpl->vertexCount;
		float* fogCoordsBuffer = new float[ vboImpl->fogCoordsSize ];
		Float3 posCamera = Engine::instance().getScene3DMgr().getCamera().getPosition();
		for (int i = 0; i < vboImpl->vertexCount; ++i)
		{
			Float3 pos(vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
			float dist = pos.distanceTo(posCamera) / 1000.f;
			fogCoordsBuffer[i] = dist > 1.f ? 1.f : dist;
		}

		glGenBuffersARB( 1, &vboImpl->idFogCoords );
		glBindBufferARB( GL_ARRAY_BUFFER, vboImpl->idFogCoords );
		glBufferDataARB( GL_ARRAY_BUFFER, vboImpl->fogCoordsSize*sizeof(float), fogCoordsBuffer, GL_STATIC_DRAW );

		delete [] fogCoordsBuffer;
	}
}

//---------------------------------------------------------------------

void VBO::draw() const
{
	if (!isExtensionSupport) return;

	glFrontFace(vboImpl->m_quadsInsteadOfTriangles?GL_CCW:GL_CW);
/*
	#define BUFFER_OFFSET(i) ((void*)(i))
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboImpl->idVertices);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(0));   //The starting point of the VBO, for the vertices
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(12));   //The starting point of normals, 12 bytes away
  //glClientActiveTexture(GL_TEXTURE0);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(24));   //The starting point of texcoords, 24 bytes away
 */
/*
//Define this somewhere in your header file
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

  glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboImpl->idVertices);
  glEnableVertexAttribArray(0);    //We like submitting vertices on stream 0 for no special reason
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), BUFFER_OFFSET(0));   //The starting point of the VBO, for the vertices
  glEnableVertexAttribArray(1);    //We like submitting normals on stream 1 for no special reason
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), BUFFER_OFFSET(12));     //The starting point of normals, 12 bytes away
  glEnableVertexAttribArray(2);    //We like submitting texcoords on stream 2 for no special reason
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), BUFFER_OFFSET(24));   //The starting point of texcoords, 24 bytes away
 */
	//Thread::claimSemaphore();

	glBindBufferARB( GL_ARRAY_BUFFER, vboImpl->idVertices );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof(Meshes3D::BasicVertex), (void*)0);
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer(    GL_FLOAT, sizeof(Meshes3D::BasicVertex), (void*)(sizeof(float)*3));
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );//GL_TEXTURE_COORD_ARRAY_EXT
	glTexCoordPointer(2,GL_FLOAT, sizeof(Meshes3D::BasicVertex), (void*)(sizeof(float)*(3+3)));
	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer(4,   GL_FLOAT, sizeof(Meshes3D::BasicVertex), (void*)(sizeof(float)*(3+3+2)));

	if (Fog::isVolumetricFog())
	{
		glEnableClientState( GL_FOG_COORD_ARRAY );
		glBindBufferARB( GL_ARRAY_BUFFER, vboImpl->idFogCoords );
		glFogCoordPointer(GL_FLOAT, 0, (char *) NULL );
	}
	//glBindTexture(GL_TEXTURE_2D, textureId);
	if (vboImpl->idIndices == -1)
	{
		glDrawArrays( vboImpl->m_quadsInsteadOfTriangles ? GL_QUADS : GL_TRIANGLES, 0, vboImpl->vertexCount );
	}
	else
	{
		int typeIndices = -1;
		if (sizeof(TypeIndex)==sizeof(unsigned int)) typeIndices = GL_UNSIGNED_INT;
		else if (sizeof(TypeIndex)==sizeof(unsigned short)) typeIndices = GL_UNSIGNED_SHORT;
		else Utils::die();

		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, static_cast<unsigned int>(vboImpl->idIndices) );
		glDrawElements( vboImpl->m_quadsInsteadOfTriangles ? GL_QUADS : GL_TRIANGLES, vboImpl->indicesSize, typeIndices, (void*)NULL);
	}

	//-----
	// reset

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	if (Fog::isVolumetricFog()) glDisableClientState( GL_FOG_COORD_ARRAY );
	//glDisableClientState (GL_TEXTURE_COORD_ARRAY_EXT);

	// bind with 0, so, switch back to normal pointer operation
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Thread::releaseSemaphore();
}

//---------------------------------------------------------------------

VBO::~VBO()
{
	if (!isExtensionSupport) return;

	//Thread::claimSemaphore();

	//std::cout << "Removing VBO of " << vboImpl->vertexCount << " vertices." << std::endl;
	glBindBufferARB( GL_ARRAY_BUFFER, vboImpl->idVertices );
	glDeleteBuffersARB(1, &vboImpl->idVertices );
	if (Fog::isVolumetricFog())
	{
		glBindBufferARB( GL_ARRAY_BUFFER, vboImpl->idFogCoords );
		glDeleteBuffersARB(1, &vboImpl->idFogCoords );
	}

	delete vboImpl;

	//Thread::releaseSemaphore();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
