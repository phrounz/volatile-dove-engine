
#if !defined(USES_WINDOWS8_DESKTOP) && !defined(USES_WINDOWS8_METRO)
	#include "opengl/opengl_inc.h"
#endif

#include "../include/Engine.h"

#include "../include/MouseManager.h"


#if (defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)) && !defined(USES_SDL_INSTEAD_OF_GLUT)

	const MouseManager::ButtonType MouseManager::MOUSE_LEFT_BUTTON	= GLUT_LEFT_BUTTON;
	const MouseManager::ButtonType MouseManager::MOUSE_MIDDLE_BUTTON = GLUT_MIDDLE_BUTTON;
	const MouseManager::ButtonType MouseManager::MOUSE_RIGHT_BUTTON	= GLUT_RIGHT_BUTTON;
	//const int MouseManager::MOUSE_WHEEL_UP      = GLUT_WHEEL_UP;
	//const int MouseManager::MOUSE_WHEEL_DOWN    = GLUT_WHEEL_DOWN;

#else

	const MouseManager::ButtonType MouseManager::MOUSE_LEFT_BUTTON = 0;
	const MouseManager::ButtonType MouseManager::MOUSE_MIDDLE_BUTTON = 1;
	const MouseManager::ButtonType MouseManager::MOUSE_RIGHT_BUTTON = 2;

#endif


static void (* mouseEventFunction)(int,bool,int,int) = NULL;
static void (* mouseMotionFunction)(const Int2&,bool) = NULL;

//---------------------------------------------------------------------

MouseManager::MouseManager()
{
	this->initMouseAsNotPressed();
}

//---------------------------------------------------------------------

void MouseManager::initMouseAsNotPressed()
{
	for (int i = 0; i < 5; ++i)
	{
		mouseButton[i] = false;
		mouseButtonBefore[i] = false;
	}
}

//---------------------------------------------------------------------
//function called when a mouse button is pressed

void MouseManager::onPointerPressed(ButtonType button, int x, int y)
{
    if (mouseEventFunction != NULL) mouseEventFunction(button, true, x, y);
	int i = getButtonTableIdFromMouseId(button);
	mouseButton[i] = true;
	mouseButtonBefore[i] = true;
}

//---------------------------------------------------------------------
//function called when a mouse button is released

void MouseManager::onPointerReleased(ButtonType button, int x, int y)
{
	if (mouseEventFunction != NULL) mouseEventFunction(button, true, x, y);
	int i = getButtonTableIdFromMouseId(button);
	mouseButton[i] = false;
	mouseButtonBefore[i] = false;
}

//---------------------------------------------------------------------
//functions called when the mouse is moved

void MouseManager::onPointerMoving(bool isClicking, int x, int y)
{
	mouseRealPosition.data[0] = x;
	mouseRealPosition.data[1] = y;
    if (mouseMotionFunction != NULL) mouseMotionFunction(mouseRealPosition, isClicking);
	//mousePosition = convertPositionInWindowToVirtualPosition(mouseRealPosition);
	mousePosition = mouseRealPosition;
}

//---------------------------------------------------------------------

void MouseManager::onPointerWheelChanged(int wheelDelta, int x, int y)
{
	//if (mouseEventFunction != NULL) mouseEventFunction(button, true, x, y);
	wheel += wheelDelta;
}

//---------------------------------------------------------------------

bool MouseManager::getMouseButton(int n)
{
	return mouseButton[ getButtonTableIdFromMouseId(n) ];
}

//---------------------------------------------------------------------

int MouseManager::getButtonTableIdFromMouseId(int n)
{
	switch (n)
	{
		case MouseManager::MOUSE_LEFT_BUTTON:	return 0;
		case MouseManager::MOUSE_MIDDLE_BUTTON:	return 1;
		case MouseManager::MOUSE_RIGHT_BUTTON:	return 2;
		//case OpenGL::MOUSE_WHEEL_UP:		return 3;
		//case OpenGL::MOUSE_WHEEL_DOWN:		return 4;
		//default:Assert(false);
	}
	return 0;
}

//---------------------------------------------------------------------

std::string MouseManager::convertFromButtonToStr(MouseManager::ButtonType button)
{
	std::string strbutton;
	if (button == MouseManager::MOUSE_LEFT_BUTTON)
		strbutton = "LEFT_BUTTON";
	else if (button == MouseManager::MOUSE_MIDDLE_BUTTON)
		strbutton = "MIDDLE_BUTTON";
	else if (button == MouseManager::MOUSE_RIGHT_BUTTON)
		strbutton = "RIGHT_BUTTON";
	else if (button == 5)//MouseManager::MOUSE_WHEEL_UP)
		strbutton = "LEFT_BUTTON_PLUS_WHEEL_UP";
	else if (button == 6)//MouseManager::MOUSE_WHEEL_DOWN)
		strbutton = "RIGHT_BUTTON_PLUS_WHEEL_UP";
	//else
	//	Assert(false);
	return strbutton;
}

//---------------------------------------------------------------------
