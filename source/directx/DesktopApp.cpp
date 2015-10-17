#ifdef USES_WINDOWS8_DESKTOP

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "uuid.lib")
//#pragma comment(lib, "c:/windows/system32/Dxgidebug.dll")
//#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "windowscodecs.lib")

#include <windowsx.h>

#define INITGUID 
#include "DXGIDebug.h"

#include "../AppSetup.h"
#include "../EngineError.h"
#include "DesktopWindow.h"

#include "DesktopApp.h"

//---------------------------------------------------------------------

DesktopApp::DesktopApp(HINSTANCE hInstance, int nCmdShow, AbstractMainClass* abstractMainClass) :m_isCrashedState(false)
{
	m_mainClass = abstractMainClass;

	desktopAppHandlerHelp.father = this;

	DesktopWindow* window = new DesktopWindow(hInstance, nCmdShow, &desktopAppHandlerHelp, 
		AppSetup::instance().isWindowFullscreen(), 
		AppSetup::instance().getWindowCurrentRealSize(), 
		AppSetup::instance().getWindowTitle().c_str());

	DirectXBase::Initialize(window, NULL, -1, -1);
}

//---------------------------------------------------------------------

void DesktopApp::onMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!Engine::instance().isInit()) return;
	if (message == WM_SIZE)
	{
		UpdateForWindowSizeChange();
		Engine::instance().onWindowResizeInternals();
		m_mainClass->onWindowResize();

		//case WM_SIZING:
		//	m_mainClass->onResizeWindowInternals(rc->right - rc->left, rc->bottom - rc->top);
		//	m_mainClass->onWindowResize();
		//	UpdateForWindowSizeChange();
		//	break;

	}
	else if (message == WM_SYSCOMMAND)
	{
		switch (wParam)
		{
		case SC_CLOSE:
			//m_mainClass->onSuspending();
			break;
		}
	}
	else
	{
		if (m_isCrashedState) return;
		m_desktopAppControls.onMessage(message, wParam, lParam, m_mainClass, reinterpret_cast<const DesktopWindow*>(getWindow())->getHwnd());
	}
}

//---------------------------------------------------------------------

void DesktopApp::Run()
{
	Assert(!Engine::instance().isInit());
	Engine::instance().initLowLevel();

	try
	{
		m_mainClass->init();
		DirectXBase::startComputingFrameDuration();

		MSG msg = { 0 };

		while (true)
		{
			// ------------ update

			if (PeekMessage(&msg, reinterpret_cast<const DesktopWindow*>(getWindow())->getHwnd(), 0, 0, PM_REMOVE))
			{
				//if (!GetMessage(&msg, NULL, 0, 0))return msg.wParam;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				//if (WM_QUIT != msg.message) break;
				continue;
			}

			bool needProcessAgain = m_mainClass->update();
			Engine::instance().getSoundMgr().manage();

			if (reinterpret_cast<const DesktopWindow*>(getWindow())->mustBeDestroyed() || !Engine::instance().isRunning()) break;

			// ------------ render

			this->BeginDraw();
			m_mainClass->render();
			this->EndDraw();
			Engine::instance().m_frameDuration = DirectXBase::computeFrameDuration();

			if (reinterpret_cast<const DesktopWindow*>(getWindow())->mustBeDestroyed() || !Engine::instance().isRunning()) break;
		}

		m_mainClass->deinit();
		Engine::instance().deinitLowLevel();
	}
	catch (EngineError e)
	{
		outputln("Caught EngineError");
		if (this->isDrawing()) this->EndDraw();
		m_isCrashedState = true;

		MSG msg = { 0 };

		while (true)
		{
			if (PeekMessage(&msg, reinterpret_cast<const DesktopWindow*>(getWindow())->getHwnd(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}

			this->BeginDraw();
			Engine::instance().clearScreen(CoreUtils::colorBlack);
			Engine::instance().getScene2DMgr().drawText(NULL, e.getFullText().c_str(), Int2(20, 40), 18, CoreUtils::colorWhite);
			this->EndDraw();
			Engine::instance().m_frameDuration = DirectXBase::computeFrameDuration();
		}
	}

	cleanup();

	uninitializeWindow();

	outputln("======== CLEANUP DONE ========");

#ifndef NDEBUG
	// http://gamedev.stackexchange.com/questions/14633/what-do-these-state-creation-warnings-mean-in-the-dx11-debug-output
	typedef HRESULT(__stdcall *fPtr)(const IID&, void**);
	HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
	fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");
	IDXGIDebug* pDxgiDebug;
	DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDxgiDebug);
	pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
}

//---------------------------------------------------------------------

DesktopApp::~DesktopApp()
{
	//s_directXMainObjects->uninitializeWindow();
}

//---------------------------------------------------------------------

#endif //USES_WINDOWS8_DESKTOP
