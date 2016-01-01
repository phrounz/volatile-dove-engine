#pragma once
#ifdef USES_WINDOWS8_DESKTOP

#include "../../include/AbstractMainClass.h"
#include "../FrameDurationCounter.h"

#include "DirectXBase.h"
#include "DesktopWindow.h"
#include "DesktopAppControls.h"

ref class DesktopApp sealed: public DirectXBase
{
	friend class DesktopWindow;
public:
	virtual void Run();
internal:
	DesktopApp(AbstractMainClass* abstractMainClass);
	void onMessage(UINT message, WPARAM wParam, LPARAM lParam);
private:
	~DesktopApp();
	class DesktopAppHandlerHelp : public DesktopWindow::DesktopWindowMessageHandler
	{
	public:
		void onMessage(UINT message, WPARAM wParam, LPARAM lParam) { father->onMessage(message, wParam, lParam); }
		DesktopApp^ father;
	};
	void manageEvents();

	AbstractMainClass* m_mainClass;
	DesktopAppHandlerHelp desktopAppHandlerHelp;
	bool m_isCrashedState;

	DesktopAppControls m_desktopAppControls;
	FrameDurationCounter m_frameDurationCounter;
	bool m_suspended;
};

#endif //USES_WINDOWS8_DESKTOP