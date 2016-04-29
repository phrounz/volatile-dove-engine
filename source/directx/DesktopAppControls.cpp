#ifdef USES_WINDOWS8_DESKTOP
#include <windowsx.h>

#include "../../include/AbstractMainClass.h"
#include "../../include/Engine.h"

#include "DesktopWindow.h"
#include "DesktopAppControls.h"

//--------------------------------------------------------------------------------------

void DesktopAppControls::onMessage(UINT message, WPARAM wParam, LPARAM lParam, AbstractMainClass* mainClass, HWND windowHandle)
{
	int key = -1;
	bool isPressed = false;
	POINT pt;
	//RECT* rc = (LPRECT)lParam;
	switch (message)
	{
	case WM_KEYDOWN:
		key = convertKeyForKeyboardManager(wParam);
		Engine::instance().onKeyPressedInternals(key);
		mainClass->onKeyPressed(key);
		break;
	case WM_KEYUP:
		key = convertKeyForKeyboardManager(wParam);
		Engine::instance().onKeyReleasedInternals(key);
		mainClass->onKeyReleased(key);
		break;
	case WM_LBUTTONDOWN:
		Engine::instance().onPointerPressedInternals(MouseManager::MOUSE_LEFT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerPressed(MouseManager::MOUSE_LEFT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONDOWN:
		Engine::instance().onPointerPressedInternals(MouseManager::MOUSE_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerPressed(MouseManager::MOUSE_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDOWN:
		Engine::instance().onPointerPressedInternals(MouseManager::MOUSE_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerPressed(MouseManager::MOUSE_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		Engine::instance().onPointerReleasedInternals(MouseManager::MOUSE_LEFT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerReleased(MouseManager::MOUSE_LEFT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONUP:
		Engine::instance().onPointerReleasedInternals(MouseManager::MOUSE_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerReleased(MouseManager::MOUSE_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONUP:
		Engine::instance().onPointerReleasedInternals(MouseManager::MOUSE_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerReleased(MouseManager::MOUSE_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEMOVE:
		isPressed = ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) ? true : false);
		Engine::instance().onPointerMovedInternals(isPressed, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		mainClass->onPointerMoved(isPressed, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL:
		// http://stackoverflow.com/questions/29915639/why-get-x-lparam-does-return-an-absolute-position-on-mouse-wheel
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(windowHandle, &pt);
		mainClass->onPointerWheelChanged(GET_WHEEL_DELTA_WPARAM(wParam), pt.x, pt.y);// / WHEEL_DELTA
		break;
	}
}

//--------------------------------------------------------------------------------------

int DesktopAppControls::convertKeyForKeyboardManager(size_t wParam)
{
	switch (wParam)
	{
	case VK_F1:			return KeyboardManager::KEY_F1;
	case VK_F2:			return KeyboardManager::KEY_F1;
	case VK_F3:			return KeyboardManager::KEY_F3;
	case VK_F4:			return KeyboardManager::KEY_F4;
	case VK_F5:			return KeyboardManager::KEY_F5;
	case VK_F6:			return KeyboardManager::KEY_F6;
	case VK_F7:			return KeyboardManager::KEY_F7;
	case VK_F8:			return KeyboardManager::KEY_F8;
	case VK_F9:			return KeyboardManager::KEY_F9;
	case VK_F10:		return KeyboardManager::KEY_F10;
	case VK_F11:		return KeyboardManager::KEY_F11;
	case VK_F12:		return KeyboardManager::KEY_F12;
	case VK_LEFT:		return KeyboardManager::KEY_LEFT;
	case VK_RIGHT:		return KeyboardManager::KEY_RIGHT;
	case VK_UP:			return KeyboardManager::KEY_UP;
	case VK_DOWN:		return KeyboardManager::KEY_DOWN;
	case VK_TAB:		return KeyboardManager::KEY_TABULATION;
	case VK_RETURN:		return KeyboardManager::KEY_ENTER;
	case VK_SHIFT:		return KeyboardManager::KEY_SHIFT;
	case VK_CONTROL:	return KeyboardManager::KEY_CONTROL;
		//case VK_PAGEUP: return KEY_PAGEUP;
		//case VK_PAGEDOWN: return KEY_PAGEDOWN;
	case VK_ESCAPE:		return KeyboardManager::KEY_ESCAPE;
	case VK_DELETE:		return KeyboardManager::KEY_DEL;
	default: return static_cast<int>(wParam);
	}
}

#endif