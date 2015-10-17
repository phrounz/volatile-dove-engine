#ifdef USES_WINDOWS8_DESKTOP

#include <map>

#include "DesktopWindow.h"

#include "../../include/Utils.h"
#include "../../include/KeyboardManager.h"

namespace
{
	std::map<HWND, DesktopWindow*> s_hwnds;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	auto it = s_hwnds.find(hWnd);

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		if (it != s_hwnds.end())
		{
			DesktopWindow* desktopWindow = it->second;
			desktopWindow->m_mustBeDestroyed = true;
		}
		break;

	default:
		if (it != s_hwnds.end())
		{
			DesktopWindow* desktopWindow = it->second;
			desktopWindow->onMessage(message, wParam, lParam);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//--------------------------------------------------------------------------------------

DesktopWindow::DesktopWindow(HINSTANCE hInstance, int nCmdShow, DesktopWindowMessageHandler* messageReceiver, bool isFullscreen, const Int2& windowSize, const std::string& windowTitle)
:m_messageReceiver(messageReceiver), m_mustBeDestroyed(false)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(5 + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
	if (!RegisterClassEx(&wcex))
		Assert(false);

	// Create window
	RECT rc = { 0, 0, windowSize.width(), windowSize.height() };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE); ;
	HWND hwnd = CreateWindow(L"TutorialWindowClass", Utils::convertStringToWString(windowTitle).c_str(), isFullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW,
		// WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	Assert(hwnd != nullptr);

	ShowWindow(hwnd, isFullscreen ? SW_MAXIMIZE : nCmdShow);

	// resize to take into consideration the border size
	/*RECT rcClient, rcWind;
	POINT ptDiff;
	GetClientRect(hwnd, &rcClient);
	GetWindowRect(hwnd, &rcWind);
	ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
	ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
	MoveWindow(hwnd, rcWind.left, rcWind.top, (rc.right - rc.left) + ptDiff.x, (rc.bottom - rc.top) + ptDiff.y, TRUE);*/

	m_hwnd = hwnd;

	s_hwnds[hwnd] = this;
}

//--------------------------------------------------------------------------------------

Windows::Foundation::Rect DesktopWindow::getBounds() const
{
	RECT rc;
	//GetWindowRect(m_hwnd, &rc);
	GetClientRect(m_hwnd, &rc);
	return Windows::Foundation::Rect((float)rc.left, (float)rc.top, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top));
}

//--------------------------------------------------------------------------------------

void DesktopWindow::onMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_messageReceiver->onMessage(message, wParam, lParam);
}

//--------------------------------------------------------------------------------------

HRESULT DesktopWindow::createSwapChainForWindow(
	IDXGIFactory2* dxgiFactory,
	IUnknown *d3dDevice,
	const DXGI_SWAP_CHAIN_DESC1* swapChainDesc,
	IDXGISwapChain1** pSwapChain)
{
	return dxgiFactory->CreateSwapChainForHwnd(d3dDevice, this->getHwnd(), swapChainDesc, nullptr, nullptr, pSwapChain);
}

//--------------------------------------------------------------------------------------

DesktopWindow::~DesktopWindow()
{
	DestroyWindow(m_hwnd);
}

//--------------------------------------------------------------------------------------

#endif //USES_WINDOWS8_DESKTOP

