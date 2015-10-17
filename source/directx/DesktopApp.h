#pragma once
#ifdef USES_WINDOWS8_DESKTOP

#include "../../include/AbstractMainClass.h"

#include "DirectXBase.h"
#include "DesktopWindow.h"
#include "DesktopAppControls.h"

ref class DesktopApp sealed: public DirectXBase
{
	friend class DesktopWindow;
public:
	virtual void Run();
internal:
	DesktopApp(HINSTANCE hInstance, int nCmdShow, AbstractMainClass* abstractMainClass);
	void onMessage(UINT message, WPARAM wParam, LPARAM lParam);
private:
	~DesktopApp();
	class DesktopAppHandlerHelp : public DesktopWindow::DesktopWindowMessageHandler
	{
	public:
		void onMessage(UINT message, WPARAM wParam, LPARAM lParam) { father->onMessage(message, wParam, lParam); }
		DesktopApp^ father;
	};

	AbstractMainClass* m_mainClass;
	DesktopAppHandlerHelp desktopAppHandlerHelp;
	bool m_isCrashedState;

	DesktopAppControls m_desktopAppControls;
};

#endif //USES_WINDOWS8_DESKTOP