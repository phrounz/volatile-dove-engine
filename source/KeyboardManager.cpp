/******************************************************************
Done by Francois Braud
******************************************************************/

#ifdef _MSC_VER
	#include <Windows.h>
#endif

#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
	#include "opengl/opengl_inc.h"
#endif

#include "../include/Utils.h"
#include "../include/Engine.h"

#include "../include/KeyboardManager.h"

// don't forget to modify NB_KEYBOARD_KEYS if you add keys

//---------------------------------------------------------------------

void (* keyEventFunction)(int) = NULL;

//---------------------------------------------------------------------
//function called when a key is pressed with state down

void KeyboardManager::onKeyPressed(int key)
{
	//Engine::instance().onKeyPressed((int)key);
    if (keyEventFunction != NULL)
        keyEventFunction(key);
    this->keys[key] = true;
}

//---------------------------------------------------------------------
//function called when a key is pressed with state up

void KeyboardManager::onKeyReleased(int key)
{
	this->keysWaitForNoRepeat[key] = false;
    this->keys[key] = false;
}

//---------------------------------------------------------------------
void KeyboardManager::setKeyPressedEventFunction(void (* eventFunction_)(int))
{
    keyEventFunction = eventFunction_;
}

//---------------------------------------------------------------------

void KeyboardManager::initKeysAsNotPressed()
{
    for (int i = 0; i < NB_KEYBOARD_KEYS; ++i)
	{
        this->keys[i] = false;
		this->keysWaitForNoRepeat[i] = false;
	}
}

//---------------------------------------------------------------------

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)

bool KeyboardManager::getModifier(int key, bool waitForNoRepeat) const
{
#ifdef USES_SDL_INSTEAD_OF_GLUT
	#pragma message("TODO KeyboardManager::getModifier SDL")
#else
	int modifiers = glutGetModifiers();
	if (key == KEY_SHIFT) return ((modifiers & GLUT_ACTIVE_SHIFT) != 0);
	else if (key == KEY_CONTROL) return ((modifiers & GLUT_ACTIVE_CTRL) != 0);
	else if (key == KEY_ALT) return ((modifiers & GLUT_ACTIVE_ALT) != 0);
	else
	{
		std::stringstream keystr;
		keystr << "file=" << __FILE__ << " line=" << __LINE__ << "; key= " << key;
		AssertMessage(false, keystr.str());
	}
#endif
	return false;
}

#endif

//---------------------------------------------------------------------

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

// http://msdn.microsoft.com/en-us/library/windows/apps/windows.system.virtualkey.aspx

int KeyboardManager::convertFromWin8(int virtualKey)
{
	switch (virtualKey)
	{
		case 37:return KEY_LEFT;
		case 38:return KEY_UP;
		case 39:return KEY_RIGHT;
		case 40:return KEY_DOWN;
		case 33:return KEY_PAGEUP;
		case 34:return KEY_PAGEDOWN;
		case  9:return KEY_TABULATION;
		case 46:return KEY_DEL;
		case 13:return KEY_ENTER;
		case 27:return KEY_ESCAPE;
		case 16:return KEY_SHIFT;
		case 17:return KEY_CONTROL;
		case 164:return KEY_ALT;//alt missing? taking leftmenu instead
		case 107:return KEY_NUMPAD_PLUS;
		case 109:return KEY_NUMPAD_MINUS;
		case 110:return KEY_NUMPAD_DECIMAL;
		case 111:return KEY_NUMPAD_DIVIDE;
	}
	// F1 to F12
	if (virtualKey >= 112 && virtualKey <= 123) return virtualKey + KEY_F1 - 112;
	// 0-9 same that ascii, ok
	// A-Z same that ascii, ok
	return virtualKey;
}

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)

Key KeyboardManager::convertFromGlut(int key)
{
	switch (key) {
#ifdef USES_SDL_INSTEAD_OF_GLUT
	#pragma message("TODO KeyboardManager::convertFromGlut SDL")
#else
		case GLUT_KEY_LEFT:		return KeyboardManager::KEY_LEFT;
        case GLUT_KEY_RIGHT:	return KeyboardManager::KEY_RIGHT;
        case GLUT_KEY_UP:		return KeyboardManager::KEY_UP;
        case GLUT_KEY_DOWN:		return KeyboardManager::KEY_DOWN;
        case GLUT_KEY_PAGE_UP:	return KeyboardManager::KEY_PAGEUP;
        case GLUT_KEY_PAGE_DOWN:return KeyboardManager::KEY_PAGEDOWN;
		case GLUT_KEY_F1:		return KeyboardManager::KEY_F1;
		case GLUT_KEY_F2:		return KeyboardManager::KEY_F2;
		case GLUT_KEY_F3:		return KeyboardManager::KEY_F3;
		case GLUT_KEY_F4:		return KeyboardManager::KEY_F4;
		case GLUT_KEY_F5:		return KeyboardManager::KEY_F5;
		case GLUT_KEY_F6:		return KeyboardManager::KEY_F6;
		case GLUT_KEY_F7:		return KeyboardManager::KEY_F7;
		case GLUT_KEY_F8:		return KeyboardManager::KEY_F8;
		case GLUT_KEY_F9:		return KeyboardManager::KEY_F9;
		case GLUT_KEY_F10:		return KeyboardManager::KEY_F10;
		case GLUT_KEY_F11:		return KeyboardManager::KEY_F11;
		case GLUT_KEY_F12:		return KeyboardManager::KEY_F12;
#endif
    }
	return -1;
}

#endif

Key KeyboardManager::convertFromStrToKey(const std::string& chars)
{
	Key asciiCode = 0;
	if (chars.size() > 1 && chars.substr(0,1) == "F") // "F1" to "F12" keys
	{
		std::string rest = chars.substr(1);
		asciiCode = KEY_F1 - 1 + atoi(rest.c_str());
	}
	else if (chars.size() > 1 && chars.substr(0,6) == "ASCII:")
	{
		std::string rest = chars.substr(6);
		asciiCode = atoi(rest.c_str());
	}
	else if (chars == "RIGHT")  asciiCode = 28;
	else if (chars == "LEFT")   asciiCode = 29;
	else if (chars == "TOP")    asciiCode = 30;
	else if (chars == "BOTTOM") asciiCode = 31;
	else
	{
		AssertMessage(chars.size() == 1, (std::string("Unknown key code: ")+chars).c_str());
		asciiCode = (int)chars[0];
	}
	return asciiCode;
}

std::string KeyboardManager::convertFromKeyToStr(Key key)
{
	std::stringstream keysstr;
	if (key >= 256)
	{
		keysstr << "F" << (key-255);
	}
	else if (key == 28) keysstr << "RIGHT";
	else if (key == 29) keysstr << "LEFT";
	else if (key == 30) keysstr << "UP";
	else if (key == 31) keysstr << "DOWN";
	else if (key >= 128 || key < 32)
	{
		keysstr << "ASCII:" << key;
	}
	else
	{
		keysstr << (char)key;
	}
	return keysstr.str();
}


//-------------------------------------------------------------------------
#if defined(_MSC_VER) && !defined(USES_WINDOWS8_METRO)

namespace
{
	// https://ubikann.com/2009/07/12/change-keyboard-from-english-to-french-at-login-screen-on-windows-7/

	std::pair<std::string, std::string> getKeyboardLayoutNameAndIso3166Name(const HKL& hkl)
	{
		wchar_t szBuf[512];
		DWORD temp = (UINT)hkl & 0xffffffff;
		// http://stackoverflow.com/questions/12364659/retrieving-language-name-for-keyboard-layout-from-win-xp-os-using-win-api
		GetLocaleInfo(MAKELCID(temp,SORT_DEFAULT), LOCALE_SLANGUAGE , szBuf, 512);
		std::pair<std::string, std::string> output;
		output.first = Utils::convertWStringToString(std::wstring(szBuf));
		GetLocaleInfo(MAKELCID(temp,SORT_DEFAULT), LOCALE_SISO3166CTRYNAME, szBuf, 512);
		output.second = Utils::convertWStringToString(std::wstring(szBuf));
		return output;
	}
}

std::pair<std::string, std::string> KeyboardManager::getCurrentKeyboardLayoutNameAndIso3166Name()
{
	return getKeyboardLayoutNameAndIso3166Name(GetKeyboardLayout(0));
}

bool KeyboardManager::activateKeyboardLayoutIfAvailable(const std::vector<std::string>& countryIso3166Codes)
{
	std::pair<std::string, std::string> currentKeyboardLayoutInfos = getCurrentKeyboardLayoutNameAndIso3166Name();
	iterateVectorConst(countryIso3166Codes, std::string)
	{
		if ((*it) == currentKeyboardLayoutInfos.second)
			return false;// not needed, we already use one of the requested layouts
	}

	HKL hkls[256];
	int res = GetKeyboardLayoutList(255, hkls);
	if (res > 0)
	{
		//wchar_t szBuf[512];
		for (int i = 0; i < res; i++) // switch between "active" keyboard layouts
		{
			std::pair<std::string, std::string> out = getKeyboardLayoutNameAndIso3166Name(hkls[i]);
			//outputln("Keyboard:" << out.first << ";" << out.second);
			iterateVectorConst(countryIso3166Codes, std::string)
			{
				if ((*it) == out.second)
				{
					HKL res2 = ActivateKeyboardLayout(hkls[i], 0);
					if (res2 == 0)
					{
						outputln("ERROR: Failed changing keyboard layout");
					}
					return true;
				}
			}
		}
	}
	return false;
}
#else
std::pair<std::string, std::string> KeyboardManager::getCurrentKeyboardLayoutNameAndIso3166Name()
{
#pragma message("TODO getCurrentKeyboardLayoutNameAndIso3166Name")
	return std::pair<std::string, std::string>();
}
bool KeyboardManager::activateKeyboardLayoutIfAvailable(const std::vector<std::string>& countryIso3166Codes)
{
#pragma message("TODO activateKeyboardLayoutIfAvailable")
	return false;
}
#endif

//---------------------------------------------------------------------
