#pragma once

#include <wrl.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#ifndef VERSION_WINDOWS_8_0
#include <DXGI1_3.h>
#endif
#include <DirectXMath.h>

#include <map>

#include "AbstractWindow.h"

//-----------------------------------------------------------------------------------

class WinStoreAppWindow : public AbstractWindow
{
public:
	WinStoreAppWindow(Windows::UI::Core::CoreWindow^ coreWindow) : m_coreWindow(coreWindow) {}
	virtual Windows::Foundation::Rect getBounds() const { return m_coreWindow->Bounds; }
	Windows::UI::Core::CoreWindow^ getCoreWindow() { return m_coreWindow; }

	HRESULT createSwapChainForWindow(
		IDXGIFactory2* dxgiFactory,
		IUnknown *d3dDevice,
		const DXGI_SWAP_CHAIN_DESC1* desc,
		IDXGISwapChain1** pSwapChain);

	~WinStoreAppWindow() {}

private:
	Windows::UI::Core::CoreWindow^ m_coreWindow;
};

//-----------------------------------------------------------------------------------
