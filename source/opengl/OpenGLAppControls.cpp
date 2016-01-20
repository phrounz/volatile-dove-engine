
#ifdef _MSC_VER
	#include <Windows.h>
	#include <WinGDI.h>
#endif
#include <GL/glut.h>
#ifndef USES_JS_EMSCRIPTEN
	#include <GL/freeglut_ext.h>
#endif

#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

//---------

#include "../../include/CoreUtils.h"
#include "../../include/Engine.h"
#include "../AppSetup.h"

#include "OpenGLAppControls.h"

//---------------------------------------------------------------------
// static values

namespace
{
	AbstractMainClass* s_mainClass = NULL;
	bool s_eventHappened = false;
	bool s_hasPressedAltF4 = false;
}

//---------------------------------------------------------------------
//function called when a mouse button is pressed

void OpenGLApp_onPointerPressedOrReleased(int button, int state,int x, int y)
{
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2(x,y));
	if (button==GLUT_WHEEL_UP)
	{
		s_mainClass->onPointerWheelChanged(120, pos.x(), pos.y());
	}
	else if (button==GLUT_WHEEL_DOWN)
	{
		s_mainClass->onPointerWheelChanged(-120, pos.x(), pos.y());
	}
	else if (state==GLUT_DOWN)
	{
		Engine::instance().onPointerPressedInternals(button, x, y);
		s_mainClass->onPointerPressed(button, pos.x(), pos.y());
	}
	else if (state==GLUT_UP)
	{
		Engine::instance().onPointerReleasedInternals(button, x, y);
		s_mainClass->onPointerReleased(button, pos.x(), pos.y());
	}
	s_eventHappened = true;
}

//---------------------------------------------------------------------
//functions called when the mouse is moved

void OpenGLApp_onPointerMovingActive(int x,int y)
{
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2(x,y));
	Engine::instance().onPointerMovedInternals(true, pos.x(), pos.y());
	s_mainClass->onPointerMoved(true, pos.x(), pos.y());
}

void OpenGLApp_onPointerMovingPassive(int x,int y)
{
	Int2 pos = AppSetup::instance().convertRealPositionToVirtualPosition(Int2(x,y));
	Engine::instance().onPointerMovedInternals(false, pos.x(), pos.y());
	s_mainClass->onPointerMoved(false, pos.x(), pos.y());
}

//---------------------------------------------------------------------
//function called when a key is pressed with state down

void OpenGLApp_onKeyDown(unsigned char key,int x, int y)
{
	Engine::instance().onKeyPressedInternals((int)key);
	s_mainClass->onKeyPressed((int)key);
	s_eventHappened = true;
}

void OpenGLApp_onKeyDown(int key,int x, int y)
{
	Engine::instance().onKeyPressedInternals((int)key);
	s_mainClass->onKeyPressed((int)key);
	s_eventHappened = true;
	if (key == KeyboardManager::KEY_F4 && Engine::instance().getKeyboardMgr().getKey(KeyboardManager::KEY_ALT))
	{
		s_hasPressedAltF4 = true;
	}
}

//---------------------------------------------------------------------
//function called when a key is pressed with state up

void OpenGLApp_onKeyUp(unsigned char key,int x, int y)
{
	Engine::instance().onKeyReleasedInternals((int)key);
	s_mainClass->onKeyReleased((int)key);
	s_eventHappened = true;
}

void OpenGLApp_onKeyUp(int key,int x, int y)
{
	Engine::instance().onKeyReleasedInternals((int)key);
	s_mainClass->onKeyReleased((int)key);
	s_eventHappened = true;
}

//---------------------------------------------------------------------
//function called when a special key is pressed with state down

void OpenGLApp_onKeyDownSpecial(int key, int x, int y)
{
	Key newkey = KeyboardManager::convertFromGlut(key);
	if (newkey != -1) OpenGLApp_onKeyDown(newkey,x,y);
}

//---------------------------------------------------------------------
//function called when a key special is pressed with state up

void OpenGLApp_onKeyUpSpecial(int key, int x, int y)
{
	Key newkey = KeyboardManager::convertFromGlut(key);
	if (newkey != -1) OpenGLApp_onKeyUp(newkey,x,y);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

void OpenGLAppControls::initControls(AbstractMainClass* mainClass)
{	
	this->resetEventHappenedToken(false);
	s_mainClass = mainClass;

	glutMouseFunc(OpenGLApp_onPointerPressedOrReleased);
	glutMotionFunc(OpenGLApp_onPointerMovingActive);
    glutPassiveMotionFunc(OpenGLApp_onPointerMovingPassive);

	glutKeyboardFunc(OpenGLApp_onKeyDown);
	glutKeyboardUpFunc(OpenGLApp_onKeyUp);
    glutSpecialFunc(OpenGLApp_onKeyDownSpecial);
    glutSpecialUpFunc(OpenGLApp_onKeyUpSpecial);
}

//---------------------------------------------------------------------

bool OpenGLAppControls::hasPressedAltF4()
{
	return s_hasPressedAltF4;
}

//---------------------------------------------------------------------

void OpenGLAppControls::resetEventHappenedToken(bool newValue)
{
	s_eventHappened = newValue;
}

//---------------------------------------------------------------------

bool OpenGLAppControls::hasEventHappenedSinceReset() const
{
	return s_eventHappened;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
