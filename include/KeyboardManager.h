#ifndef KeyboardManager_h_INCLUDED
#define KeyboardManager_h_INCLUDED

#include <string>
#include <vector>
#include <utility>

typedef int Key;

//---------------------------------------------------------------------
/**
* \brief The keyboard inputs manager (singleton).
*/

class KeyboardManager
{
	friend class Engine;
public:

static const Key KEY_LEFT		= 29;
static const Key KEY_RIGHT		= 28;
static const Key KEY_UP			= 30;
static const Key KEY_DOWN		= 31;
static const Key KEY_PAGEUP		= 33;
static const Key KEY_PAGEDOWN	= 34;
static const Key KEY_BACKSPACE	= 8;
static const Key KEY_TABULATION	= 9;
static const Key KEY_DEL		= 127;
static const Key KEY_ENTER		= 13;
static const Key KEY_ESCAPE		= 27;

//note: F1 to F12 keys are beyond 256
static const Key KEY_F1		= 256;
static const Key KEY_F2		= 257;
static const Key KEY_F3		= 258;
static const Key KEY_F4		= 259;
static const Key KEY_F5		= 260;
static const Key KEY_F6		= 261;
static const Key KEY_F7		= 262;
static const Key KEY_F8		= 263;
static const Key KEY_F9		= 264;
static const Key KEY_F10	= 265;
static const Key KEY_F11	= 266;
static const Key KEY_F12	= 267;

static const Key KEY_SHIFT		= 268;
static const Key KEY_CONTROL	= 269;
static const Key KEY_ALT	    = 270;

static const Key KEY_NUMPAD_PLUS	= 271;
static const Key KEY_NUMPAD_MINUS	= 272;
static const Key KEY_NUMPAD_DECIMAL	= 273;
static const Key KEY_NUMPAD_DIVIDE	= 274;

#if defined(USES_WINDOWS8_DESKTOP) || defined(USES_WINDOWS8_METRO)

	static int convertFromWinDesktopMessage(unsigned int wParam);
	static int convertFromWin8(int virtualKey);

#elif defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX) || defined(USES_JS_EMSCRIPTEN)

	static Key convertFromGlut(int key);

#endif

	//! defines the function which will be called at each keyboard event
	void setKeyPressedEventFunction(void (* eventFunction_)(Key));

	//! is this key (between 0 et NB_KEYBOARD_KEYS-1) is pressed
	inline bool getKey(Key key)
	{
#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
			if (key>=KEY_SHIFT) return getModifier(key, false);
#endif
		return keys[key];
	}

	//! is this key (between 0 et NB_KEYBOARD_KEYS-1) is pressed (with no distinction between 'A' and 'a')
	inline bool getKeyNoCase(unsigned char key) const
	{
#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
			if (key>=KEY_SHIFT) return getModifier(key, false);
#endif
		return keys[tolower(key)] || keys[toupper(key)];
	}

	//! if is this key (between 0 et NB_KEYBOARD_KEYS-1) is pressed, return true once until the key is up.
	inline bool getKeyNoRepeat(unsigned char key)
	{
#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
			if (key>=KEY_SHIFT) return getModifier(key, true);
#endif
		if (keys[key] && !keysWaitForNoRepeat[key])
		{
			keysWaitForNoRepeat[key] = true;
			return true;
		}
		return false;
	}

	//! Reset all the keys as if they were not pressed. May be useful is there is a keyboard glitch.
    void initKeysAsNotPressed();

	// utility function for user string representation
	// supports: 
	// - "a" means "a"
	// - "ASCII:11" means tabulation
	// - "F3" means F3 key
	static Key convertFromStrToKey(const std::string& chars);
	static std::string convertFromKeyToStr(Key key);

	//! Functions related to keyboard layout
	static std::pair<std::string, std::string> getCurrentKeyboardLayoutNameAndIso3166Name();
	static bool activateKeyboardLayoutIfAvailable(const std::vector<std::string>& countryIso3166Codes);
	
private:

	KeyboardManager() {this->initKeysAsNotPressed();}
	~KeyboardManager() {}
		
#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
		//! renvoie le modificateurs d'etat appuye pour shift, ctrl, ou alt
		bool getModifier(int key, bool waitForNoRepeat) const;
#endif
	
	void onKeyPressed(int key);
	void onKeyReleased(int key);

	static const int NB_KEYBOARD_KEYS = 275;

	bool keys[NB_KEYBOARD_KEYS]; // 256 ascii keys, followed by 12 keys from F1 to F12, followed by shift, ctrl, alt
	bool keysWaitForNoRepeat[NB_KEYBOARD_KEYS]; // same here
};

//---------------------------------------------------------------------

#endif //KeyboardManager_h_INCLUDED
