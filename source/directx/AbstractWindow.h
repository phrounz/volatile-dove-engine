#pragma once

#include <windows.h>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

//#include "../../include/AbstractMainClass.h"

//-----------------------------------------------------------------------------------

class AbstractWindow
{
public:
	//! Get the position and size of the window
	virtual Windows::Foundation::Rect getBounds() const = 0;

	virtual HRESULT createSwapChainForWindow(
		IDXGIFactory2* dxgiFactory,
		IUnknown *d3dDevice,
		const DXGI_SWAP_CHAIN_DESC1* desc,
		IDXGISwapChain1** pSwapChain) = 0;

	virtual ~AbstractWindow() {}
};

//-----------------------------------------------------------------------------------
