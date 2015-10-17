#ifdef USES_WINDOWS8_METRO_WITH_XAML

#include <wrl.h>
#include <wrl\client.h>

#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11_2.h>

#include <DirectXMath.h>

#include "windows.ui.xaml.media.dxinterop.h"

#include <vector>

//-------------------------

#include "../../include/AbstractMainClass.h"

#include "CommonDirectXOptions.h"
#include "Multisampling.h"

#include "DirectXHelper.h"
#include "DirectXDraw.h"
#include "BasicReaderWriter.h"

#include "AbstractWindow.h"

#include "WinStoreAppWindow.h"
#include "../AppSetup.h"
#include "../EngineError.h"

//-------------------------

#include "ScenarioImageSource.h"

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;
using namespace DirectX;
using namespace ScenarioComponent;

AbstractMainClass* createAbstractMainClass(const std::vector<std::string>& arguments);

ScenarioImageSource::ScenarioImageSource(int pixelWidth, int pixelHeight, bool isOpaque, Platform::String^ language)
	:SurfaceImageSource(pixelWidth, pixelHeight, isOpaque), m_crashedMessage("")
{
	std::vector<std::string> arguments;
	arguments.push_back("test");
	m_mainClass = createAbstractMainClass(arguments);
	
	AbstractWindow* abstractWindow = new WinStoreAppWindow(CoreWindow::GetForCurrentThread());
	m_directXBaseFinal.Initialize(abstractWindow, reinterpret_cast<IUnknown*>(this), pixelWidth, pixelHeight);
	
	//Initialize(abstractWindow);
	
	Assert(!Engine::instance().isInit());
	Engine::instance().initLowLevel();

	m_winStoreAppControls.initControls(CoreWindow::GetForCurrentThread(), m_mainClass);

	try
	{
		m_mainClass->init();
	}
	catch (EngineError e)
	{
		this->catchEngineError(e);
	}
	
	Application::Current->Suspending += ref new SuspendingEventHandler(this, &ScenarioImageSource::OnSuspending);
	Application::Current->Resuming += ref new EventHandler<Platform::Object^>(this, &ScenarioImageSource::OnResuming);

	//CoreWindow::GetForCurrentThread()->SizeChanged +=
	//	ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &WinStoreApp::OnWindowSizeChanged);

	m_directXBaseFinal.startComputingFrameDuration();
}

void ScenarioImageSource::restart()
{
	m_mainClass->deinit();
	Engine::instance().deinitLowLevel();
	
	Engine::instance().initLowLevel();
	try
	{
		m_mainClass->init();
	}
	catch (EngineError e)
	{
		this->catchEngineError(e);
	}

}

// Begins drawing.
void ScenarioImageSource::update()
{
	if (m_crashedMessage.size() == 0)
	{
		try
		{
			m_mainClass->update();
			Engine::instance().getSoundMgr().manage();
		}
		catch (EngineError e)
		{
			this->catchEngineError(e);
		}
	}
}

// Begins drawing.
void ScenarioImageSource::BeginDraw()
{
	try
	{
		if (Windows::UI::Core::CoreWindow::GetForCurrentThread()->Visible)
			m_directXBaseFinal.BeginDraw();
	}
	catch (EngineError e)
	{
		this->catchEngineError(e);
	}
}

// Clears the surface with the given color.  This must be called 
// after BeginDraw and before EndDraw for a given update.
void ScenarioImageSource::Clear(Windows::UI::Color color)
{
	if (Windows::UI::Core::CoreWindow::GetForCurrentThread()->Visible)
		m_directXBaseFinal.Clear(m_crashedMessage.size() > 0 ? Windows::UI::Colors::Black : color);
}

void ScenarioImageSource::RenderNextAnimationFrame()
{
	if (Windows::UI::Core::CoreWindow::GetForCurrentThread()->Visible)
	{
		if (m_crashedMessage.size() > 0)
		{
			Engine::instance().getScene2DMgr().set2DMode();
			Engine::instance().getScene2DMgr().drawText(NULL, m_crashedMessage.c_str(), Int2(20, 40), 18, CoreUtils::colorWhite);
		}
		else
		{
			try
			{
				m_mainClass->render();
			}
			catch (EngineError e)
			{
				this->catchEngineError(e);
			}
		}
	}
}

// Ends drawing updates started by a previous BeginDraw call.
void ScenarioImageSource::EndDraw()
{
	if (Windows::UI::Core::CoreWindow::GetForCurrentThread()->Visible)
	{
		m_directXBaseFinal.EndDraw();
		Engine::instance().m_frameDuration = m_directXBaseFinal.computeFrameDuration();
	}
}

void ScenarioImageSource::OnSuspending(Object ^sender, SuspendingEventArgs ^args)
{    
	outputln("OnSuspending");
	m_directXBaseFinal.OnSuspending(sender, args);

	// Insert your code here
	m_mainClass->onSuspending();
	Engine::instance().getSoundMgr().onSuspending();
}

void ScenarioImageSource::OnResuming(Object^ sender, Object^ args)
{
	m_directXBaseFinal.OnResuming(sender, args);
}

void ScenarioImageSource::OnWindowSizeChanged(int width, int height)
{
	m_directXBaseFinal.UpdateForWindowSizeChange();
	Engine::instance().onWindowResizeInternals();
	m_mainClass->onWindowResize();
}

void ScenarioImageSource::catchEngineError(const EngineError& e)
{
	outputln("Caught EngineError");
	m_winStoreAppControls.setCrashedState();// disable mouse and key events
	m_crashedMessage = e.getFullText();
}

#endif
