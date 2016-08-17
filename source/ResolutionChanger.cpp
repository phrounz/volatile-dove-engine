
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#if defined(USES_JS_EMSCRIPTEN)

namespace ResolutionChanger
{
	bool changeResolution(int width, int height)
	{
		// no way to change the resolution
		return false;
	}
}

#elif defined(USES_LINUX)

//#define CHANGERESOLUTION_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#if CHANGERESOLUTION_LINUX
#include <X11/extensions/Xrandr.h>
#endif

#include "ResolutionChanger.h"

namespace ResolutionChanger
{
	// http://www.cplusplus.com/forum/unices/108795/

	void getDesktopResolution(int* horizontal, int* vertical)
	{
		Display* disp = XOpenDisplay(NULL);
		Screen*  scrn = DefaultScreenOfDisplay(disp);
		printf("%d %d\n", scrn->width, scrn->height);
		*horizontal = scrn->width;
		*vertical = scrn->height;
	}


	/*void testResolutions()
	{
		Display* dpy = XOpenDisplay(NULL);// connect to X server
		Window root = RootWindow(dpy, 0);// get root window id

		int num_sizes;
		XRRScreenSize* xrrs = XRRSizes(dpy, 0, &num_sizes);//get possible screen resolutions

		short possible_frequencies[64][64];

		for(int i = 0; i < num_sizes; i ++)
		{
			printf("\n\t%2i : %4i x %4i   (%4imm x%4imm ) ", i, xrrs[i].width, xrrs[i].height, xrrs[i].mwidth, xrrs[i].mheight);

			int num_rates;
			short* rates = XRRRates(dpy, 0, i, &num_rates);

			for (int j = 0; j < num_rates; j ++)
			{
				possible_frequencies[i][j] = rates[j];
				printf("%4i ", rates[j]);
			}
		}

		printf("\n");

		// get current resolution and frequency
		XRRScreenConfiguration  *conf = XRRGetScreenInfo(dpy, root);
		short original_rate = XRRConfigCurrentRate(conf);
		Rotation original_rotation;
		SizeID original_size_id  = XRRConfigCurrentConfiguration(conf, &original_rotation);

		printf("\n\tCURRENT SIZE ID  : %i\n", original_size_id);
		printf("\tCURRENT ROTATION : %i \n", original_rotation);
		printf("\tCURRENT RATE     : %i Hz\n\n", original_rate);

		// change resolution
		printf("\tCHANGED TO %i x %i PIXELS, %i Hz\n\n", xrrs[1].width, xrrs[1].height, possible_frequencies[1][0]);
		XRRSetScreenConfigAndRate(dpy, conf, root, 1, RR_Rotate_0, possible_frequencies[1][0], CurrentTime);

		// sleep a while
		usleep(6000000);

		// restore original resolution
		printf("\tRESTORING %i x %i PIXELS, %i Hz\n\n", xrrs[original_size_id].width, xrrs[original_size_id].height, original_rate);
		XRRSetScreenConfigAndRate(dpy, conf, root, original_size_id, original_rotation, original_rate, CurrentTime);

		XCloseDisplay(dpy);// exit
	}*/

	//-------------------------------------------------------------------------

	bool changeResolution(int width, int height)
	{
#if CHANGERESOLUTION_LINUX
		Display* dpy = XOpenDisplay(NULL);// connect to X server
		Window root = RootWindow(dpy, 0);// get root window id

		int num_sizes;
		XRRScreenSize* xrrs = XRRSizes(dpy, 0, &num_sizes);//get possible screen resolutions

		short possible_frequencies[64][64];

		int idRes = -1;

		for(int i = 0; i < num_sizes; i ++)
		{
			//printf("\n\t%2i : %4i x %4i   (%4imm x%4imm ) ", i, xrrs[i].width, xrrs[i].height, xrrs[i].mwidth, xrrs[i].mheight);

			if (width == xrrs[i].width && height == xrrs[i].height)
			{
				idRes = i;
			}

			int num_rates;
			short* rates = XRRRates(dpy, 0, i, &num_rates);

			for (int j = 0; j < num_rates; j ++)
			{
				possible_frequencies[i][j] = rates[j];
				//printf("%4i ", rates[j]);
			}
		}

		//printf("\n");

		// get current resolution and frequency
		XRRScreenConfiguration  *conf = XRRGetScreenInfo(dpy, root);
		short original_rate = XRRConfigCurrentRate(conf);
		Rotation original_rotation;
		SizeID original_size_id  = XRRConfigCurrentConfiguration(conf, &original_rotation);

		//printf("\n\tCURRENT SIZE ID  : %i\n", original_size_id);
		//printf("\tCURRENT ROTATION : %i \n", original_rotation);
		//printf("\tCURRENT RATE     : %i Hz\n\n", original_rate);

		// change resolution
		if (idRes != -1)
		{
			printf("\tCHANGED TO %i x %i PIXELS, %i Hz\n\n", xrrs[1].width, xrrs[1].height, possible_frequencies[1][0]);
			XRRSetScreenConfigAndRate(dpy, conf, root, idRes, RR_Rotate_0, possible_frequencies[1][0], CurrentTime);
		}

		XCloseDisplay(dpy);// exit

		return (idRes != -1);
#endif
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#elif USES_WINDOWS8_METRO

#include <wrl/client.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <agile.h>
#include <windows.ui.xaml.media.dxinterop.h>

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace MetroHelpers;
using namespace Platform;
using namespace D2D1;

#include "ResolutionChanger.h"

// http://blogs.microsoft.co.il/tomershamam/2012/07/24/get-screen-resolution-in-windows-8-metro-style-application/

namespace ResolutionChanger
{
#warning "not tested ResolutionChanger::getDesktopResolution"
	void getDesktopResolution(int* horizontal, int* vertical)
	{
		ComPtr<IDXGIDevice> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		IDXGIOutput * pOutput;
		if (dxgiAdapter->EnumOutputs(0, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);
			*horizontal = desc.DesktopCoordinates.right;
			*vertical = desc.DesktopCoordinates.bottom;
		}
	}

	bool changeResolution(int width, int height)
	{
		// no way to change the resolution
		return false;
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#else // !USES_LINUX

#include <Windows.h>
#include <wtypes.h>
#include <iostream>

#include "ResolutionChanger.h"

namespace ResolutionChanger
{
	// http://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c

	void getDesktopResolution(int* horizontal, int* vertical)
	{
	   RECT desktop;
	   const HWND hDesktop = GetDesktopWindow();// get desktop window handle
	   GetWindowRect(hDesktop, &desktop);// get size of screen
	   // top left corner have coordinates (0,0) and bottom right corner will have coordinates (horizontal, vertical)
	   *horizontal = desktop.right;
	   *vertical = desktop.bottom;
	}

	bool changeResolution(int width, int height)
	{
		// http://www.gamedev.net/topic/602791-c-change-screen-resolution/
		// http://www.codeproject.com/Articles/15072/Programmatically-change-display-resolution

		DEVMODE devMode;
		memset(&devMode, 0, sizeof(devMode));

		EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devMode);
		devMode.dmPelsWidth = width;
		devMode.dmPelsHeight = height;
		LONG res = ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_FULLSCREEN, NULL);
		if (res == DISP_CHANGE_SUCCESSFUL)
		{
			SendMessage(HWND_BROADCAST, WM_DISPLAYCHANGE, (WPARAM)32, MAKELPARAM(width, height));
		}
		return (res == DISP_CHANGE_SUCCESSFUL);
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#endif
