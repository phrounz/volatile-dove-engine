
#include "WinStoreAppWindow.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace D2D1;

HRESULT WinStoreAppWindow::createSwapChainForWindow(
	IDXGIFactory2* dxgiFactory,
	IUnknown *d3dDevice,
	const DXGI_SWAP_CHAIN_DESC1* desc,
	IDXGISwapChain1** pSwapChain)
{
	return dxgiFactory->CreateSwapChainForCoreWindow(
		d3dDevice,
		reinterpret_cast<IUnknown*>(this->getCoreWindow()),
		desc,
		nullptr, // allow on all displays
		pSwapChain
		);
}
