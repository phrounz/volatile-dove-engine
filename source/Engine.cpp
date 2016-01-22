
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined (USES_JS_EMSCRIPTEN)
	#include "opengl/OpenGL.h"
#else
	#include "directx/DXMain.h"
#endif

#include "../include/Engine.h"
#include "../include/FileUtil.h"
#include "Steam.h"

#include "AppSetup.h"

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	void OpenGL_onResizeWindow(int w, int h);//declared in OpenGL.h
#endif

//---------------------------------------------------------------------

Engine* Engine::m_instance = NULL;

//---------------------------------------------------------------------

Engine::Engine(const char* argv0, const AppSetupInfos& appSetupInfos, const std::vector<SteamAchievementInfo>& steamAchievementInfos)
:
m_frameDuration(0), m_argv0(argv0), m_isInit(false), m_isAppRunning(true), m_appSetup(new AppSetup(appSetupInfos))
{
#ifdef USES_STEAM_INTEGRATION
	std::vector<Steam::AchievementInfo> achievementInfos;
	for (int i = 0; i < steamAchievementInfos.size(); ++i)
		achievementInfos.push_back(Steam::AchievementInfo(steamAchievementInfos[i].id, steamAchievementInfos[i].name, steamAchievementInfos[i].description));
	Steam::init(achievementInfos);
#endif

#ifndef USES_WINDOWS8_METRO
	FileUtil::setAppDataFolderBasename(m_appSetup->getAppSetupInfos().appDataFolderBasename.c_str());
#endif
	m_instance = this;
}

//---------------------------------------------------------------------

Engine::~Engine()
{
	delete m_appSetup;
#ifdef USES_STEAM_INTEGRATION
	Steam::deinit();
#endif

}

//---------------------------------------------------------------------

void Engine::initLowLevel()
{
	outputln("Init Engine...");
	Utils::indentLog();
	Utils::initClock();
	Utils::initRandomSeed();
	m_appSetup->init();
	m_scene2D = new Scene2D;
	m_keyboardManager = new KeyboardManager;
	m_mouseManager = new MouseManager;
	m_joystickManager = new JoystickManager;
#ifdef USES_SOUND
	m_soundManager = new SoundManager(m_argv0.c_str());
#endif 
#ifdef USES_SCENE3D
	m_scene3D = new Scene3D;
#endif
	m_isInit = true;
	Utils::unindentLog();
	outputln("Init Engine done.");

	this->onWindowResizeInternals();
}

//---------------------------------------------------------------------

void Engine::updateInternals()
{
#ifdef USES_SOUND
	Engine::instance().getSoundMgr().manage();
#endif
#ifdef USES_STEAM_INTEGRATION
	Steam::runStep();
#endif
}

//---------------------------------------------------------------------

void Engine::unlockSteamAchievement(std::string str)
{
#ifdef USES_STEAM_INTEGRATION
	Steam::unlockAchievement(str);
#endif
}

//---------------------------------------------------------------------

void Engine::clearSteamAchievement(std::string str)
{
#ifdef USES_STEAM_INTEGRATION
	Steam::clearAchievement(str);
#endif
}

//---------------------------------------------------------------------

void Engine::resetSoundSystem()
{
#ifdef USES_SOUND
	delete m_soundManager;
	m_soundManager = new SoundManager(m_argv0.c_str());
#endif
}

//---------------------------------------------------------------------

void Engine::deinitLowLevel()
{
#ifdef USES_SCENE3D
	delete m_scene3D;
#endif

#ifdef USES_SOUND
	delete m_soundManager;
#endif

	delete m_joystickManager;
	delete m_keyboardManager;
	delete m_mouseManager;
	delete m_scene2D;

	m_appSetup->deinit();
}

void Engine::clearScreen(const Color& color)
{
	this->getScene2DMgr().clearScreen(color);
#if (defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)) && defined(USES_SCENE3D)
	this->getScene3DMgr().clearScene(color);
#endif
}

Color Engine::getDefaultBackgroundColor()
{
	return AppSetup::instance().getDefaultBackgroundColor();
}

Color Engine::getDefaultForegroundColor()
{
	return AppSetup::instance().getDefaultForegroundColor();
}

void Engine::startRender()
{
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)
	//AppSetup::manage();
	//this->openGL->manageOpenGL();
	this->getScene2DMgr().set2DMode();
#else
	DXMain::instance()->RunStepBegin();
#endif
}

void Engine::endRender()
{
	AppSetup::instance().manageRender();
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	DXMain::instance()->RunStepEnd();
#endif
}
//---------------------------------------------------------------------

void Engine::onPointerPressedInternals(int button, int x, int y)
{
	m_mouseManager->onPointerPressed(button, x, y);
}

void Engine::onPointerReleasedInternals(int button, int x, int y)
{
	m_mouseManager->onPointerReleased(button, x, y);
}

void Engine::onPointerMovedInternals(bool isClicking, int x, int y)
{
	m_mouseManager->onPointerMoving(isClicking, x, y);
}

void Engine::onPointerWheelChangedInternals(int wheelDelta, int x, int y)
{
	m_mouseManager->onPointerWheelChanged(wheelDelta, x, y);
}

void Engine::onKeyPressedInternals(int virtualKey)
{
	m_keyboardManager->onKeyPressed(virtualKey);
}

void Engine::onKeyReleasedInternals(int virtualKey)
{
	m_keyboardManager->onKeyReleased(virtualKey);
}

void Engine::onWindowResizeInternals()
{
#if (defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)) && defined(USES_SCENE3D)
	m_scene3D->createWindowSizeDependentResources();
#endif
}

//---------------------------------------------------------------------
