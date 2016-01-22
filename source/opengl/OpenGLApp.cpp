
#include "opengl_inc.h"

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
	//static const int MAX_FPS = 60;
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
	m_mainClass = abstractMainClass;

	//---------------
	// init window and graphic system

	std::string argv0str = argv0;
	char* argvCustom[2];
	argvCustom[0] = new char[argv0str.size()+1];
	strncpy(argvCustom[0], argv0str.c_str(), argv0str.size()+1);
	argvCustom[1] = NULL;
	int argcCustom = 1;
#ifndef USES_SDL_INSTEAD_OF_GLUT
	glutInit(&argcCustom,argvCustom);
#endif
}

//---------------------------------------------------------------------

void OpenGLApp::manageEvents()
{
	// events are automatically dispatched by glut

	// resize window if required
	Int2 newSizeWindow = AppSetup::instance().getWindowRealSize();
	if (newSizeWindow != AppSetup::instance().getWindowCurrentRealSize())
	{
		this->onResizeWindow(newSizeWindow.width(), newSizeWindow.height());
	}
}

//---------------------------------------------------------------------

void OpenGLApp::BeginDraw()
{
	AppSetup::instance().manageRender();
}

void OpenGLApp::EndDraw()
{
}

//---------------------------------------------------------------------

void oneIter(void* openGLAppInstance)
{
	OpenGLApp* that = (OpenGLApp*)openGLAppInstance;

	that->manageEvents();

	bool needProcessAgain = that->m_mainClass->update();
	Assert (needProcessAgain);
	that->BeginDraw();
	that->m_mainClass->render();
	that->EndDraw();
	Engine::instance().m_frameDuration = that->m_frameDurationCounter.retrieve();
	Engine::instance().updateInternals();
}

//---------------------------------------------------------------------

void OpenGLApp::run()
{	
	// ------------- init Engine objects
	Assert(!Engine::instance().isInit());
	Engine::instance().initLowLevel();
#ifndef __EMSCRIPTEN__
	AppSetup::instance().onResizeWindow(Int2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)));
#endif
	m_openGLAppControls.initControls(m_mainClass);

	m_frameDurationCounter.init();

	try
	{
		// ------------- call main class init()
		m_mainClass->init();

		AppSetup::instance().manageRender();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(oneIter, (void*)this, 60, 1);
#else

		while (Engine::instance().isRunning() && !m_openGLAppControls.hasPressedAltF4()) // ------------ MAIN LOOP
		{
			// ------------ manage events

			this->manageEvents();

			// ------------- call main class update()

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
					this->manageEvents();
					AppSetup::instance().manageRender();
				}
			}
			else
			{
				// ------------ call main class render
				this->BeginDraw();
				m_mainClass->render();
				this->EndDraw();
			}

			Engine::instance().m_frameDuration = m_frameDurationCounter.retrieve();
			Engine::instance().updateInternals();
		}
#endif

	}
	catch (EngineError e)
	{
		outputln("Caught EngineError");
		m_isCrashedState = true;

		while (true)
		{
			this->BeginDraw();
			Engine::instance().clearScreen(CoreUtils::colorBlack);
			Engine::instance().getScene2DMgr().drawText(NULL, e.getFullText().c_str(), Int2(20, 40), 18, CoreUtils::colorWhite);
			this->EndDraw();
			Engine::instance().m_frameDuration = m_frameDurationCounter.retrieve();
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
