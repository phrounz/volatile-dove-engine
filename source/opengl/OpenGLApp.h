#ifndef OpenGLApp_h_INCLUDED
#define OpenGLApp_h_INCLUDED

#include "../../include/CoreUtils.h"
#include "../FrameDurationCounter.h"

#include "OpenGLAppControls.h"

class Engine;

class OpenGLApp
{
public:
	OpenGLApp(AbstractMainClass* abstractMainClass, const char* argv0);
	void run();
	~OpenGLApp();
private:
	OpenGLAppControls m_openGLAppControls;
	bool m_isCrashedState;
	AbstractMainClass* m_mainClass;
	FrameDurationCounter m_frameDurationCounter;

	void onResizeWindow(int w, int h);
	void manageEvents();
	void BeginDraw();
	void EndDraw();
};

#endif //OpenGLApp_h_INCLUDED
