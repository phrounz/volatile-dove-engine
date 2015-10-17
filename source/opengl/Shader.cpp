
#include <iostream>
#include <fstream>

#define GL_GLEXT_PROTOTYPES
#include <cstdlib>
#ifdef _MSC_VER
	#include <Windows.h>
	#include <WinGDI.h>

	#include <GL/glew.h>
#endif
#include <GL/glut.h>
#ifdef _MSC_VER
	#include <GL/glext.h>
#endif

#if defined(USES_WINDOWS_OPENGL)
	#include <windows.h>
	#include <gl/GL.h>
	#include <gl/GLU.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
#elif defined (USES_LINUX)
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <GL/freeglut_ext.h>
#else
	#error
#endif

#include "../../include/FileUtil.h"

#include "Shader.h"

//---------------------------------------------------------------------

namespace
{
	// GL ERROR CHECK
	int checkGLError(char *file, int line)
	{
	   int retCode = 0;
	   GLenum glErr = glGetError();
	   while (glErr != GL_NO_ERROR) 
	   {
		 if (gluErrorString(glErr))
		 {
			outputln("GL Error #" << glErr << "(" << gluErrorString(glErr) << ") " << " in File " << file << " at line: " << line);
		 }
		 else
		 {
			outputln("GL Error #" << glErr << " (no message available)" << " in File " << file << " at line: " << line);
		 }
			
		 retCode = 1;
		 glErr = glGetError();
	   }
	   return retCode;
	}

}

#define CHECK_GL_ERROR() checkGLError(__FILE__, __LINE__)

Shader* Shader::s_instance = NULL;

//---------------------------------------------------------------------

Shader::Shader(bool areShadersSupported):m_userDefinedTextureId(0), m_areShadersSupported(areShadersSupported), m_isUsing(false)
{
	s_instance = this;

	if (areShadersSupported)
	{
		Utils::indentLog("Initializing shaders ...");
		size_t vertexShaderLength;
		char* vertexShaderSource = (char*)FileUtil::readFile(FileUtil::APPLICATION_FOLDER, "./shaders/vertexshader.glsl", &vertexShaderLength);

		size_t fragmentShaderLength;
		char* fragmentShaderSource = (char*)FileUtil::readFile(FileUtil::APPLICATION_FOLDER, "./shaders/fragmentshader.glsl", &fragmentShaderLength);

		bool res = compileShaders((char*)vertexShaderSource, vertexShaderLength, (char*)fragmentShaderSource, fragmentShaderLength);
		if (!res) areShadersSupported = false;
		//Assert(res);

		delete [] vertexShaderSource;
		delete [] fragmentShaderSource;

		Utils::unindentLog("Initializing shaders done.");
	}
	if (!areShadersSupported)
	{
		outputln("WARNING: Shaders not supported.");
	}
}

//---------------------------------------------------------------------

bool Shader::compileShaders(const char* vertexShaderSource, int vertexShaderLength, const char* fragmentShaderSource, int fragmentShaderLength)
{
	Assert(sizeof(int) == sizeof(GLint) && sizeof(GLhandleARB) == sizeof(unsigned int));

	m_vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	m_fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSourceARB(m_vertexShaderObject, 1, &vertexShaderSource, &vertexShaderLength);
	glShaderSourceARB(m_fragmentShaderObject, 1, &fragmentShaderSource, &fragmentShaderLength);

	glCompileShaderARB(m_vertexShaderObject);
	glCompileShaderARB(m_fragmentShaderObject);

	GLint compiled1, compiled2;
	glGetObjectParameterivARB(m_vertexShaderObject, GL_COMPILE_STATUS, &compiled1);
	glGetObjectParameterivARB(m_fragmentShaderObject, GL_COMPILE_STATUS, &compiled2);
	if (compiled1 && compiled2)
	{
   		m_programObject = glCreateProgram();

		glAttachShader(m_programObject, m_vertexShaderObject);
		glAttachShader(m_programObject, m_fragmentShaderObject);

		glLinkProgram(m_programObject);

		GLint linked;
		glGetProgramiv(m_programObject, GL_LINK_STATUS, &linked);
		if (linked)
		{
		   return true;
		}      
	}
	else
	{
		GLint blen1 = 0;	
		GLint blen2 = 0;	
		GLsizei slen = 0;

		glGetShaderiv(m_vertexShaderObject, GL_INFO_LOG_LENGTH , &blen1);
		if (blen1 > 1)
		{
			char* compilerLog = new char[blen1];
			glGetInfoLogARB(m_vertexShaderObject, blen1, &slen, compilerLog);
			//AssertMessage(false, compilerLog);
			outputln("Vertex shader: " << compilerLog);
			delete [] compilerLog;
		}

		glGetShaderiv(m_fragmentShaderObject, GL_INFO_LOG_LENGTH , &blen2);       
		if (blen2 > 1)
		{
			char* compilerLog = new char[blen2];
			glGetInfoLogARB(m_fragmentShaderObject, blen2, &slen, compilerLog);
			//AssertMessage(false, compilerLog);
			outputln("Fragment shader: " << compilerLog);
			delete [] compilerLog;
		}
		return blen1 <= 1 && blen2 <= 1;
	}
	return false;
}

//----------------------------------------------------------------------------- 

void Shader::startUsing()
{
	if (m_areShadersSupported)
	{
		GLint cameraMatrix = glGetUniformLocation(m_programObject, "cameraMatrix");
		glUniformMatrix4fv(cameraMatrix, 1, false, (float*)&m_cameraMatrix);
		GLint fogStart = glGetUniformLocation(m_programObject, "fogStart");
		glUniform1f(fogStart, m_fogStart);
		GLint fogEnd = glGetUniformLocation(m_programObject, "fogEnd");
		glUniform1f(fogEnd, m_fogEnd);
		GLint data = glGetUniformLocation(m_programObject, "data");
		glUniform3f(data, m_data.x(), m_data.y(), m_data.z());
		GLint fogColor = glGetUniformLocation(m_programObject, "fogColor");
		glUniform4f(fogColor, m_fogColor.r()/256.f, m_fogColor.g()/256.f, m_fogColor.b()/256.f, 1.f);
		GLint usesLight = glGetUniformLocation(m_programObject, "usesLight");
		glUniform1i(usesLight, (int)m_lightEnabled);
		GLint userDefinedTextureId = glGetUniformLocation(m_programObject, "userDefinedTextureId");
		glUniform1i(userDefinedTextureId, m_userDefinedTextureId);

		glUseProgram(m_programObject);
		//CHECK_GL_ERROR();

		m_isUsing = true;
	}
}

//----------------------------------------------------------------------------- 

void Shader::stopUsing()
{
	if (m_areShadersSupported)
	{
		m_isUsing = false;
		glUseProgram(0);
		//CHECK_GL_ERROR();
	}
}

//---------------------------------------------------------------------

Shader::~Shader()
{
}

//---------------------------------------------------------------------
