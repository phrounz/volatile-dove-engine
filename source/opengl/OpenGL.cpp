
#include "opengl_inc.h"

#include "../../include/CoreUtils.h"
#include "../../include/Utils.h"
#include "../../include/Engine.h"
#ifdef USES_SCENE3D
	#include "VBO.h"
#endif

#include "OpenGL.h"

#ifdef USES_SDL_INSTEAD_OF_GLUT
	#include "SDLDraw.h"
#endif

//---------------------------------------------------------------------
/*
Int2 convertPositionInWindowToVirtualPosition(const Int2& parWindowRealPosition)
{
	return OpenGL::instance().getPixelPerPoint() == Float2(1.f,1.f) ?
		Int2(parWindowRealPosition)
		: Int2((int)(parWindowRealPosition.x() / OpenGL::instance().getPixelPerPoint().x()), (int)(parWindowRealPosition.y() / OpenGL::instance().getPixelPerPoint().y()));
}
*/
//---------------------------------------------------------------------

OpenGL::OpenGL(const char *windowTitle, const Int2& windowSize, bool isFullScreenMode, bool automaticFitToWindowSize):m_modeFullscreenGlut(false)
{
	m_ppp = Float2(1.f, 1.f);
	m_virtualPos = Int2(0,0);
	m_realWindowSize = windowSize;

	this->init2(windowTitle, windowSize);
	if (isFullScreenMode) setMode(true, -1, -1);
}

//---------------------------------------------------------------------

void OpenGL::init2(const char* windowTitle, const Int2& windowSize)
{
#ifdef USES_SDL_INSTEAD_OF_GLUT
	bool isOk = (SDL_Init(SDL_INIT_VIDEO) == 0);
	Assert(isOk);

	//m_sdlSurface = (void*)SDL_SetVideoMode( 256, 256, 32, SDL_SWSURFACE);// windowSize.width(),windowSize.height(), 24, SDL_OPENGL
	//Assert(m_sdlSurface != NULL);
	//SDLDraw::setupSDLScreen(m_sdlSurface);
	m_sdlWindow = (void*)SDL_CreateWindow(" - ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,  windowSize.width(),windowSize.height(), SDL_WINDOW_OPENGL );
	Assert(m_sdlWindow != NULL);
	m_sdlRenderer = (void*)SDL_CreateRenderer((SDL_Window*)m_sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Assert(m_sdlRenderer != NULL);
	SDLDraw::setupSDLRenderer(m_sdlRenderer);

#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	//glutInitDisplayString("rgba double depth>=32 samples>=4");
	//glutMainClassModeString( "800x600" );
	//glutEnterMainClassMode();

	glutInitWindowSize(windowSize.width(),windowSize.height());
    
	glutCreateWindow(windowTitle);
#endif

    //glutReshapeFunc(OpenGL_onResizeWindow);//DOES NOT WORK

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDisable(GL_FOG);

	glEnable(GL_ALPHA_TEST);
#ifndef USES_JS_EMSCRIPTEN
	glAlphaFunc(GL_GREATER, 0.1f);
#endif
	
	//glEnable(GL_POINT_SMOOTH);
    //glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);

	//FILE *m_File = fopen("extensions.txt", "wt");
	//const GLubyte *extensions = glGetString(GL_EXTENSIONS);
	//fprintf(m_File, "supported:%s :", extensions);

	//glEnable(GL_MULTISAMPLE_ARB);
	//void* ptrARBMultisample = wglGetProcAddress("wglChoosePixelFormatARB");//GL_ARB_multisample");
	//if (ptrARBMultisample == NULL)
	//{
	//	Utils::warningMessageToUser("wglChoosePixelFormatARB: extension not available");
	//}

#ifdef WIN32
	//PrintValue(glutGet(GLUT_WINDOW_BUFFER_SIZE));
	//does not work with freeglut?
	if (false && glutGet(GLUT_WINDOW_BUFFER_SIZE)<32)
	{
	    int n = glutGet(GLUT_WINDOW_BUFFER_SIZE);
	    char * str2 = new char[(int)floor(log10((float)n))+2];
        sprintf(str2,"%d",n);

        std::string str("\n\nThe window screen is too low (");
	    str += str2;
	    str += ")\nPlease right click on desktop - properties - settings tab, and change screen depth to 32";

	    str += "\n\nLa resolution d'affichage de windows est trop basse (";
	    str += str2;
	    str += ")\nPour y remedier, click droit sur le bureau - proprietes - parametres - et fixez la qualite couleur a 32";

        delete [] str2;

        Utils::dieErrorMessageToUser(str);
	}
#endif

    //glutReshapeWindow(windowWidth,windowHeight);
#ifdef USES_SCENE3D
	VBO::checkAndInit();
#endif
}

//---------------------------------------------------------------------

void OpenGL::set2DMode()
{
	glViewport(m_virtualPos.x(),-m_virtualPos.y(),m_realWindowSize.width(),m_realWindowSize.height());
	glFrontFace(GL_CCW);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
#ifdef USES_JS_EMSCRIPTEN
	//glOrtho(0.0, (double)m_realWindowSize.x() / (double)m_ppp.x(), (double)m_realWindowSize.y() / (double)m_ppp.y(), 0.0, 0.0, 1.0 );
#else
	glPolygonMode(GL_BACK, GL_FILL);
	glPolygonMode(GL_FRONT, GL_POINT);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluOrtho2D(0.0,(double)m_realWindowSize.x() / (double)m_ppp.x(), (double)m_realWindowSize.y() / (double)m_ppp.y(),0.0);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
#endif
}

//---------------------------------------------------------------------

void OpenGL::set3DMode(float fov, float minViewDistance, float maxViewDistance)
{
#ifdef USES_SCENE3D
	float windowAspectRatio = (float)m_realWindowSize.width() / (float)m_realWindowSize.height();

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(fov, windowAspectRatio,minViewDistance, maxViewDistance);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

//---------------------------------------------------------------------

void OpenGL::manageOpenGL(const Int2& windowSize)
{
#ifdef USES_SDL_INSTEAD_OF_GLUT
	//SDL_UpdateWindowSurface((SDL_Window*)m_sdlWindow);
	SDL_Flip((SDL_Surface*)m_sdlSurface);
#else
	//events
	//long long int periodFrame2 = Utils::getMicrosecondTime() - clockTime;

    //if (periodFrame2 < 1000000/MAX_FPS)
    //    Utils::sleepMs(int(1000/(float)MAX_FPS - (float)periodFrame2 / 1000.f));//wait when > 100 FPS

    //glFlush();
	//i64 tmpTime = Utils::getMicrosecondTime();
	glutSwapBuffers();
	//tmpTime = Utils::getMicrosecondTime() - tmpTime;

#ifdef USES_JS_EMSCRIPTEN
	glutMainLoop();
#else
	glutMainLoopEvent();
#endif
#endif

	//clockTime = Utils::getMicrosecondTime();

   // periodFrame = clockTime - timeLastFrame;
    //if (periodFrame > 200000) periodFrame = 200000;//slow down MainClass when < 5 FPS
    //timeLastFrame = clockTime;

#ifndef USES_JS_EMSCRIPTEN
    glColor3f(1.0,1.0,1.0);
    
    glRasterPos2d(0, windowSize.height());
#endif
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	//return (int)tmpTime;
}

//---------------------------------------------------------------------

#ifdef WIN32

static DISPLAY_DEVICE GetPrimaryDevice()
{
    int index=0;
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);
 
    while (EnumDisplayDevices(NULL, index++, &dd, 0))
    {
        if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) return dd;
    }
    return dd;
}


static void setHackedFullscreenMode(std::string windowTitle, int width, int height)
{
	DISPLAY_DEVICE dd = GetPrimaryDevice();
    DEVMODE dm;
    if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
    {
        printf("EnumDisplaySettings failed:%d\n", GetLastError());
    }
	ZeroMemory(&dm, sizeof(DEVMODE));
    dm.dmSize = sizeof(DEVMODE);
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
	dm.dmBitsPerPel = 32;
    dm.dmFields = (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL);
	std::cout << "Set to fullscreen mode " << width << "x" << height << std::endl;
    if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
		//AssertMessage(false, std::string(std::string("Illegal graphics mode:") + GetLastError()).c_str());
    }

	HWND hThisWnd = GetActiveWindow();//FindWindow( L"FREEGLUT", Utils::stringToWstring(windowTitle).c_str() );//GetTopWindow(GetForegroundWindow());
	if( hThisWnd )
	{
		std::cout << "Found window handle" << std::endl;
		LONG lStyle = GetWindowLong( hThisWnd, GWL_STYLE );
		SetWindowPos(hThisWnd, HWND_TOPMOST, 0,0, width, height, SWP_SHOWWINDOW);
		//Utils::sleepMs(1000);
		//SetWindowLong( hThisWnd, GWL_STYLE, WS_POPUP);//lStyle & (~WS_OVERLAPPEDWINDOW ) );
	}
}

static void setHackedWindowedMode(std::string windowTitle, int width, int height)
{
	std::cout << "Set to windowed mode " << width << "x" << height << std::endl;
	ChangeDisplaySettings(NULL, CDS_FULLSCREEN);

	HWND hThisWnd = GetActiveWindow();//FindWindow( L"FREEGLUT", Utils::stringToWstring(windowTitle).c_str() );//GetTopWindow(GetForegroundWindow());
	if( hThisWnd )
	{
		std::cout << "Found window handle" << std::endl;
		LONG lStyle = GetWindowLong( hThisWnd, GWL_STYLE );
		//SetWindowLong( hThisWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW );//lStyle & 
		//Utils::sleepMs(1000);
		SetWindowPos(hThisWnd, HWND_TOPMOST, 10,10, width, height, SWP_SHOWWINDOW);
	}
}
#endif

//---------------------------------------------------------------------

void OpenGL::setMode(bool fullscreen, int width, int height)
{
#ifdef USES_SDL_INSTEAD_OF_GLUT
	#pragma message("TODO OpenGL::setMode SDL")
#else

	if (fullscreen)
	{
		if (width == -1 && height == -1)
		{
			glutFullScreen();
			m_modeFullscreenGlut = true;
			//m_fullscreenMode = true;
		}
		else
		{
			#ifdef WIN32
				//glutEnterMainClassMode();
				setHackedFullscreenMode("", width, height);
				//OpenGL_onResizeWindow(width, height);
				m_modeFullscreenGlut = true;
				//m_fullscreenMode = true;
			#endif
		}
	}
	else
	{
		if (m_modeFullscreenGlut)
		{
			//glutReshapeWindow(width, height);
			//glutPositionWindow(10, 10);
			//glutReshapeWindow(width, height);
			//glutPositionWindow(10, 10);
			//m_fullscreenMode = false;
		}
		else
		{
			#ifdef WIN32
				setHackedWindowedMode("", width, height);
				//OpenGL_onResizeWindow(width, height);
				//m_fullscreenMode = false;
			#endif
		}
	}

#endif
}

//---------------------------------------------------------------------

Int2 OpenGL::getWindowRealSize() const
{
#ifdef USES_SDL_INSTEAD_OF_GLUT
	//return Int2(SDL_GetWindowSurface((SDL_Window*)m_sdlWindow)->w, SDL_GetWindowSurface((SDL_Window*)m_sdlWindow)->h);
	return Int2(((SDL_Surface*)m_sdlSurface)->w, ((SDL_Surface*)m_sdlSurface)->h);
#else
	return Int2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
#endif
}

//---------------------------------------------------------------------

void OpenGL::setPixelPerPoint(const Int2& windowRealSize, const Float2& ppp, const Int2& topLeftCornerInWindow)
{
	Assert(m_realWindowSize.x() > 0 && m_realWindowSize.y() > 0);

    m_ppp = ppp;
	m_virtualPos = topLeftCornerInWindow;
	m_realWindowSize = windowRealSize;

	this->set2DMode();
}

//---------------------------------------------------------------------

OpenGL::~OpenGL()
{
}

//---------------------------------------------------------------------
