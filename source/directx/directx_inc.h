
//#define USES_DX9_INSTEAD_OF_DX11

#ifdef USES_DX9_INSTEAD_OF_DX11

// TODO

//#include <d3dx9.h>
#include <direct.h>

#else

#include <wrl.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#ifndef VERSION_WINDOWS_8_0
#include <DXGI1_3.h>
#endif
#include <agile.h>

#endif
