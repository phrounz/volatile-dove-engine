#ifdef USES_WINDOWS8_DESKTOP

#ifndef DesktopWindow_h_INCLUDED
#define DesktopWindow_h_INCLUDED

#include <windows.h>

#include <string>

#include "../../include/CoreUtils.h"

#include "AbstractWindow.h"

//-----------------------------------------------------------------------------------

class DesktopWindow : public AbstractWindow
{
	friend static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	class DesktopWindowMessageHandler
	{
	public:
		virtual void onMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	};

	//! Constructor
	DesktopWindow(HINSTANCE hInstance, int nCmdShow, DesktopWindowMessageHandler* messageReceiver, bool isFullscreen, const Int2& windowSize, const std::string& windowTitle);
	
	//! Get the window handle
	HWND getHwnd() const { return m_hwnd; }
	
	//! Get the position and size of the window
	virtual Windows::Foundation::Rect getBounds() const;
	
	HRESULT createSwapChainForWindow(
		IDXGIFactory2* dxgiFactory,
		IUnknown *d3dDevice,
		const DXGI_SWAP_CHAIN_DESC1* desc,
		IDXGISwapChain1** pSwapChain);

	bool mustBeDestroyed() const { return m_mustBeDestroyed; }

	~DesktopWindow();

private:
	void onMessage(UINT message, WPARAM wParam, LPARAM lParam);
	HWND m_hwnd;
	DesktopWindowMessageHandler* m_messageReceiver;
	bool m_mustBeDestroyed;
};

//-----------------------------------------------------------------------------------

#endif //DesktopWindow_h_INCLUDED

#endif //USES_WINDOWS8_DESKTOP
