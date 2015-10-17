#pragma once
#ifdef USES_WINDOWS8_DESKTOP

#include <windowsx.h>

#include "DesktopWindow.h"

class DesktopAppControls
{
public:
	DesktopAppControls() {}
	void onMessage(UINT message, WPARAM wParam, LPARAM lParam, AbstractMainClass* mainClass, HWND windowHandle);
private:
	int convertKeyForKeyboardManager(size_t wParam);
};

#endif
