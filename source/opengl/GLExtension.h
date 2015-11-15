
#include <iostream>

#ifdef _MSC_VER
	#include <Windows.h>
	#include <WinGDI.h>

	#include <GL/glew.h>
	#include <GL/glext.h>
#endif
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#ifndef WIN32
    #include <GL/glu.h>
    #include <GL/glext.h>
    #include <GL/glx.h>
    #include <GL/glxext.h>
    //replace glXGetProcAddress by glXGetProcAddressARB if not working
    #define wglGetProcAddress(x) (void*)glXGetProcAddress((const GLubyte*)x)
#endif

#ifdef WIN32
	#include <gl/wglext.h>		//WGL extensions
	#include <gl/glext.h>		//GL extensions
#endif

#ifdef USES_LINUX
	#include <stdio.h>
	#include <string.h>
#endif

//---------------------------------------------------------------------

namespace GLExtension
{
	static bool isExtensionSupported( const char* szTargetExtension )
	{
		const unsigned char *pszExtensions = NULL;
		const unsigned char *pszStart;
		unsigned char *pszWhere, *pszTerminator;

		// Extension names should not have spaces
		pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
		if( pszWhere || *szTargetExtension == '\0' )
			return false;

		// Get Extensions String
		pszExtensions = glGetString( GL_EXTENSIONS );

		// Search The Extensions String For An Exact Copy
		pszStart = pszExtensions;
		for(;;)
		{
			pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
			if( !pszWhere )
				break;
			pszTerminator = pszWhere + strlen( szTargetExtension );
			if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
				if( *pszTerminator == ' ' || *pszTerminator == '\0' )
					return true;
			pszStart = pszTerminator;
		}
		return false;
	}
}

//---------------------------------------------------------------------
