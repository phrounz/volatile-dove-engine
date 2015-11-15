#ifdef USES_LINUX

#ifdef USES_LINUX // for changeResolution()
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <X11/Xlib.h>
	#include <X11/extensions/Xrandr.h>
#endif

namespace LinuxResolutionChanger
{
	void changeResolution()
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
	}
} // namespace LinuxResolutionChanger

#endif // USES_LINUX

