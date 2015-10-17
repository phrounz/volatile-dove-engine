/*====================================
Name: ARB_multisample.h
Author: Colt "MainRoach" McAnlis
Date: 4/29/04
Desc:
This file contains our external items

 ====================================*/

#ifndef __ARB_MULTISAMPLE_H__
#define __ARB_MULTISAMPLE_H__

#ifdef WIN32 //---------------------------------------

#include <Windows.h>

//Globals
extern bool	arbMultisampleSupported;
extern int arbMultisampleFormat;

//to check for our sampling
namespace Multisample
{
	bool initMultisample(HWND hWnd);
}

#endif //---------------------------------------

#endif

