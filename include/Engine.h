#ifndef Engine_h_INCLUDED
#define Engine_h_INCLUDED

#include <vector>
#include <utility>

#define USES_SOUND

#include "CoreUtils.h"
#include "Scene2D.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "JoystickManager.h"
#ifdef USES_SOUND
#include "SoundManager.h"
#endif
#include "Scene3D.h"
#include "AppSetupInfos.h"

class OpenGL;
class AppSetup;
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
namespace ScenarioComponent
{
	ref class ScenarioImageSource;
}
#endif

//---------------------------------------------------------------------
/**
* \brief The core engine class (singleton).
*/

class Engine
{
#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)
	friend class DesktopWindow;
	friend ref class DesktopApp;
	friend class DesktopAppControls;
	friend ref class WinStoreApp;
	friend ref class WinStoreAppControls;
	friend ref class ScenarioComponent::ScenarioImageSource;
#else
	friend class OpenGLApp;
	friend void OpenGLApp_onPointerPressedOrReleased(int button, int state, int x, int y);
	friend void OpenGLApp_onPointerMovingActive(int x, int y);
	friend void OpenGLApp_onPointerMovingPassive(int x, int y);
	friend void OpenGLApp_onKeyDown(unsigned char key, int x, int y);
	friend void OpenGLApp_onKeyDown(int key, int x, int y);
	friend void OpenGLApp_onKeyUp(unsigned char key, int x, int y);
	friend void OpenGLApp_onKeyUp(int key, int x, int y);
	//friend void OpenGLApp_onResizeWindow(int width,int height);
#endif
	friend class AbstractMainClass;
public:

	//! singleton instance retreiver - works only if there is only one instance running
	static Engine& instance() { return *m_instance; }

	bool isInit() const { return m_isInit; }


	virtual bool isRunning() const { return m_isAppRunning; }

	void startRender();
	void endRender();
	void clearScreen(const Color& color);
	Color getDefaultBackgroundColor();
	Color getDefaultForegroundColor();

	inline Scene2D& getScene2DMgr() { return *m_scene2D; }						///< get 2d display-related singleton
	inline Scene3D& getScene3DMgr() { return *m_scene3D; }						///< get 3d display-related singleton
	inline KeyboardManager& getKeyboardMgr() { return *m_keyboardManager; }		///< get keyboard-related singleton
	inline JoystickManager& getJoystickMgr() { return *m_joystickManager; }		///< get joystick-related singleton
	inline MouseManager& getMouseMgr() { return *m_mouseManager; }				///< get mouse-related singleton
#ifdef USES_SOUND
	inline SoundManager& getSoundMgr() { return *m_soundManager; }				///< get sound-related singleton
#endif
	
	inline int64_t getFrameDuration() const { return m_frameDuration; }			///< Last frame duration in miliseconds
	
	void stopRunning() { m_isAppRunning = false; }								///< Send a signal to quit the application

	void resetSoundSystem();													///< Unload and reload the sound manager

	void unlockSteamAchievement(std::string str);
	void clearSteamAchievement(std::string str);

	struct SteamAchievementInfo
	{
		SteamAchievementInfo(std::string pId, std::string pName, std::string pDescription) : id(pId), name(pName), description(pDescription) {}
		std::string id, name, description;
	};

private:
	void initLowLevel();

	//! constructor
	Engine(const char* argv0, const AppSetupInfos& initInfos, const std::vector<SteamAchievementInfo>& steamAchievementInfos);
	//! destructor
	virtual ~Engine();

	void onWindowResizeInternals();
	void deinitLowLevel();

	static Engine* m_instance;

	void updateInternals();

	void onPointerPressedInternals(int button, int x, int y);
	void onPointerReleasedInternals(int button, int x, int y);
	void onPointerMovedInternals(bool isClicking, int x, int y);
	void onPointerWheelChangedInternals(int wheelDelta, int x, int y);
	void onKeyPressedInternals(int virtualKey);
	void onKeyReleasedInternals(int virtualKey);

	KeyboardManager* m_keyboardManager;
	MouseManager* m_mouseManager;
	JoystickManager* m_joystickManager;
#ifdef USES_SOUND
	SoundManager* m_soundManager;
#endif
	Scene2D* m_scene2D;
	Scene3D* m_scene3D;

	int64_t m_frameDuration;
	std::string m_argv0;
	bool m_isInit;
	bool m_isAppRunning;

	AppSetup* m_appSetup;
};

//---------------------------------------------------------------------

#endif //Engine_h_INCLUDED
