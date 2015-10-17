
#ifdef _MSC_VER
	#include <Windows.h>
	#include <WinGDI.h>
#endif
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

//#include "../../code_highlevel/MainClass.h"

#include "../../include/KeyboardManager.h"
#include "../AppSetup.h"
#include "../EngineError.h"

#include "OpenGLApp.h"
#include "OpenGL.h"

#include "../../include/Engine.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

namespace
{
	const int MAX_FPS = 60;
}

//---------------------------------------------------------------------
// function called when the window is resized

void OpenGLApp::onResizeWindow(int w, int h)
{
	//outputln("Rescaling to (" << w << ',' << h << ")");
	if (h > 0) // h < 0 => minimized window
	{
		AppSetup::instance().onResizeWindow(Int2(w,h));
	}
	Engine::instance().onWindowResizeInternals();
	m_mainClass->onWindowResize();
	m_openGLAppControls.resetEventHappenedToken(true);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

OpenGLApp::OpenGLApp(AbstractMainClass* abstractMainClass, const char* argv0)
{
	std::string argv0str = argv0;
	char* argvCustom[2];
	argvCustom[0] = new char[argv0str.size()+1];
	strncpy(argvCustom[0], argv0str.c_str(), argv0str.size()+1);
	argvCustom[1] = NULL;
	int argcCustom = 1;

	glutInit(&argcCustom,argvCustom);

	m_mainClass = abstractMainClass;
	Assert(!Engine::instance().isInit());
	Engine::instance().initLowLevel();

	AppSetup::instance().onResizeWindow(Int2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)));
	m_openGLAppControls.initControls(abstractMainClass);
}

//---------------------------------------------------------------------

void OpenGLApp::run()
{
	try
	{
		m_mainClass->init();
		AppSetup::instance().manageRender();

		while (Engine::instance().isRunning() && !m_openGLAppControls.hasPressedAltF4())
		{
			int64_t msNow = Utils::getMillisecond();

			// resize window if required
			Int2 newSizeWindow = AppSetup::instance().getWindowRealSize();
			if (newSizeWindow != AppSetup::instance().getWindowCurrentRealSize())
			{
				this->onResizeWindow(newSizeWindow.width(), newSizeWindow.height());
			}

			// process
			bool needProcessAgain = m_mainClass->update();
			if (!needProcessAgain)
			{
				AppSetup::instance().manageRender();
				m_mainClass->render();
				// run without update or render until an event comes
				m_openGLAppControls.resetEventHappenedToken(false);
				while (!m_openGLAppControls.hasEventHappenedSinceReset())
				{
					Utils::sleepMs(20);
					Int2 newSizeWindow = AppSetup::instance().getWindowRealSize();
					if (newSizeWindow != AppSetup::instance().getWindowCurrentRealSize())
						this->onResizeWindow(newSizeWindow.width(), newSizeWindow.height());
					AppSetup::instance().manageRender();
				}
			}
			else
			{
				AppSetup::instance().manageRender();
				m_mainClass->render();
			}
			Engine::instance().m_frameDuration = Utils::getMillisecond() - msNow;
			if (1.f/(Engine::instance().m_frameDuration/1000.f) > MAX_FPS) //wait when > MAX_FPS
			{
				Utils::sleepMs(int(1000/(float)MAX_FPS - (float)Engine::instance().m_frameDuration));
				Engine::instance().m_frameDuration = (int64_t)(1000.f/(float)MAX_FPS);//10;
			}
		}
	}
	catch (EngineError e)
	{
		outputln("Caught EngineError");
		m_isCrashedState = true;

		MSG msg = { 0 };

		while (true)
		{
			int64_t msNow = Utils::getMillisecond();
			AppSetup::instance().manageRender();
			Engine::instance().clearScreen(CoreUtils::colorBlack);
			Engine::instance().getScene2DMgr().drawText(NULL, e.getFullText().c_str(), Int2(20, 40), 18, CoreUtils::colorWhite);
			Engine::instance().m_frameDuration = Utils::getMillisecond() - msNow;
		}
	}
}

//---------------------------------------------------------------------

OpenGLApp::~OpenGLApp()
{
	m_mainClass->deinit();
	Engine::instance().deinitLowLevel();
	delete m_mainClass;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
