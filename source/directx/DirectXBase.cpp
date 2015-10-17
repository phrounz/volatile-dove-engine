
#include "DXMain.h"
#include "DirectXHelper.h"
#include "../AppSetup.h"

#include "DirectXBase.h" 

//#define DEPTH_STENCIL_V2

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace D2D1;

void DirectXBase::Initialize(AbstractWindow* abstractWindow, IUnknown* imageSourceOrNull, int imageSourceWidth, int imageSourceHeight)
{ 
	m_isSuspended = false;

	new DXMain(imageSourceOrNull, imageSourceWidth, imageSourceHeight);
	DXMain::instance()->Initialize(abstractWindow);
	DXMain::instance()->SetMultisampling(AppSetup::instance().isMultisamplingEnabled());

	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();
}

void DirectXBase::startComputingFrameDuration()
{
	m_msNow = Utils::getMillisecond();
}

void DirectXBase::getWindowSize(int* width, int* height)	{ DXMain::instance()->getWindowSize(width, height); }
void DirectXBase::CreateDeviceIndependentResources()		{ DXMain::instance()->CreateDeviceIndependentResources(); }
void DirectXBase::CreateDeviceResources()					{ DXMain::instance()->CreateDeviceResources(); }
void DirectXBase::CreateWindowSizeDependentResources()		{ DXMain::instance()->CreateWindowSizeDependentResources(); DXMain::instance()->initializeDpi(); }
void DirectXBase::UpdateForWindowSizeChange()
{ 
	DXMain::instance()->UpdateForWindowSizeChange();
	int width, height;
	DXMain::instance()->getWindowSize(&width, &height);
	AppSetup::instance().onResizeWindow(Int2(width, height));
}
void DirectXBase::SetDpi(float dpi) 						{ DXMain::instance()->SetDpi(dpi); DXMain::instance()->initializeDpi(); }
void DirectXBase::Present() 								{ DXMain::instance()->Present(); }
void DirectXBase::Trim() 									{ DXMain::instance()->Trim(); }
void DirectXBase::BeginDraw() 								{ DXMain::instance()->RunStepBegin(); }
void DirectXBase::EndDraw() 								{ AppSetup::instance().manageRender(); DXMain::instance()->RunStepEnd(); }
void DirectXBase::cleanupSizeDependentResources() 			{ DXMain::instance()->cleanupSizeDependentResources(); }
void DirectXBase::cleanup() 								{ DXMain::instance()->cleanup(); }
void DirectXBase::uninitializeWindow()						{ DXMain::instance()->uninitializeWindow(); }
void DirectXBase::Clear(Windows::UI::Color color)			{ DXMain::instance()->Clear(Color(color.R, color.G, color.B, color.A)); }

void DirectXBase::OnSuspending(Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)	{ DXMain::instance()->OnSuspending(); m_isSuspended = true; }
void DirectXBase::OnResuming(Platform::Object^ sender, Platform::Object^ args)						{ m_isSuspended = false; }

const AbstractWindow* DirectXBase::getWindow() const		{ return DXMain::instance()->getWindow(); }

int64_t DirectXBase::computeFrameDuration()
{
	int64_t now = Utils::getMillisecond();
	int64_t diff = now - m_msNow;
	m_msNow = now;
	return diff;
}

bool DirectXBase::isDrawing() const { return DXMain::instance()->isDrawing(); }

DirectXBase::~DirectXBase() { delete DXMain::instance(); }
