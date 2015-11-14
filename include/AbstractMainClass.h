#ifndef AbstractMainClass_h_INCLUDED
#define AbstractMainClass_h_INCLUDED

#include "Engine.h"

//---------------------------------------------------------------------
/**
* \brief Your main class shall inherit from this class and implement all the game events.
*/

class AbstractMainClass
{
public:
	AbstractMainClass(const char* argv0, const AppSetupInfos& initInfos)
	{
		new Engine(argv0, initInfos);
	}
	//! events
	virtual void onPointerPressed(int button, int x, int y) {}
	virtual void onPointerReleased(int button, int x, int y) {}
	virtual void onPointerWheelChanged(int wheelDelta, int x, int y) {}
	virtual void onPointerMoved(bool isPressed, int x, int y) {}
	virtual void onKeyPressed(int virtualKey) {}
	virtual void onKeyReleased(int virtualKey) {}
	virtual void onSuspending() {}
	virtual void onWindowResize() {}
	virtual void onEdgeSettingsButton(const char* idStr) {}
	virtual void onSemanticZoom(float value) {}
	virtual bool getSemanticZoomIsEnabled() { return true; }

	virtual std::vector<std::pair<std::string, std::string> > getCharmSettings() { return std::vector<std::pair<std::string, std::string> >(); }

	virtual void init() = 0;					//! initialization - called at startup	
	virtual void deinit() = 0;					//! un-initialization

	virtual bool update() = 0;					//! do everything in your program
	virtual void render() = 0;					//! do any render in your program

	~AbstractMainClass()
	{
		delete &Engine::instance();
	}
};

//---------------------------------------------------------------------

#endif //AbstractMainClass_h_INCLUDED
