#ifndef ResolutionChanger_h_INCLUDED
#define ResolutionChanger_h_INCLUDED

// Change the operating system resolution
namespace ResolutionChanger
{
	void getDesktopResolution(int* horizontal, int* vertical);
	bool changeResolution(int width, int height);
};

#endif //ResolutionChanger_h_INCLUDED
