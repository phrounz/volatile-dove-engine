#ifndef AppSetupInfos_h_INCLUDED
#define AppSetupInfos_h_INCLUDED

#include <string>

#include "CoreUtils.h"

//---------------------------------------------------------------------
/**
* \brief Informations needed to set up the Engine
*/

struct AppSetupInfos
{
	AppSetupInfos() :
	windowTitle("Application"),
	windowSize(1024, 768),
	isFullscreen(false),
	changeDesktopResolution(false),
	appDataFolderBasename(L"DefaultFolder"),
	defaultFontImgPath("data/default_font.png"),
	automaticFitToWindowSize(false),
	virtualSize(-1, -1),
	virtualSizeAllowRatioDeformation(false),
	virtualSizeBorderColor(0,0,0,255),
	enableMSAA(false),
	defaultBackgroundColor(0, 0, 0, 255),
	defaultForegroundColor(255, 255, 255, 255)
	{}
	std::string windowTitle;
	Int2 windowSize;
	bool isFullscreen;
	bool changeDesktopResolution;
	std::string defaultFontImgPath;
	std::wstring appDataFolderBasename;
	bool automaticFitToWindowSize;
	Int2 virtualSize;
	bool virtualSizeAllowRatioDeformation;
	Color virtualSizeBorderColor;
	Color defaultBackgroundColor;
	Color defaultForegroundColor;
	bool enableMSAA;
};

//---------------------------------------------------------------------

#endif //AppSetupInfos_h_INCLUDED
