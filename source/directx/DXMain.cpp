
#include <Windows.h>

#include "BasicReaderWriter.h"
#include "windows.ui.xaml.media.dxinterop.h"

#include "Multisampling.h"

//#define DEPTH_STENCIL_V2

#include "AbstractWindow.h"
#include "DirectXDraw.h"

#include "DXMain.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace D2D1;

//-------------------------------------------------------------------------

DXMain* DXMain::s_instance = NULL;

//-------------------------------------------------------------------------

//:m_screenViewport(), m_scalingFactor(1) {m_supportInfo = ref new MultisamplingSupportInfo(); }
DXMain::DXMain(IUnknown* imageSourceOrNull, int imageSourceWidth, int imageSourceHeight)
: 
	m_defaultDpi(-1.f, 1.f), m_virtualPos(0, 0), m_ppp(1.f, 1.f), 
	m_imageSourceOrNull(imageSourceOrNull), m_imageSourceWidth(imageSourceWidth), m_imageSourceHeight(imageSourceHeight), 
	m_enableMultisampling(false)
{
	s_instance = this;
}

void DXMain::Initialize(AbstractWindow* window)
{
	m_isDrawing = false;
	m_window = window;
	m_dpi = static_cast<float>(Windows::Graphics::Display::DisplayProperties::LogicalDpi/*DisplayInformation::GetForCurrentView()->LogicalDpi*/);
	m_multisampling = NULL;
}

void DXMain::getWindowSize(int* width, int* height)
{
	Windows::Foundation::Rect rect = m_window->getBounds();
	*width = (int)(rect.Right - rect.Left);
	*height = (int)(rect.Bottom - rect.Top);
}

// These are the resources required independent of hardware.
void DXMain::CreateDeviceIndependentResources()
{
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	DX::ThrowIfFailed(
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, &m_d2dFactory)
		);
#ifdef ENABLE_DIRECT_WRITE
	DX::ThrowIfFailed(
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dwriteFactory)
		);
#endif
}

// These are the resources that depend on the device.
void DXMain::CreateDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering than the API default.
	// It is recommended usage, and is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> d3dDevice;
	ComPtr<ID3D11DeviceContext> d3dContext;
	DX::ThrowIfFailed(
		D3D11CreateDevice(
		nullptr,                  // specify null to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,                  // leave as nullptr unless software device
		creationFlags,            // optionally set debug and Direct2D compatibility flags
		featureLevels,            // list of feature levels this app can support
		ARRAYSIZE(featureLevels), // number of entries in above list
		D3D11_SDK_VERSION,        // always set this to D3D11_SDK_VERSION for modern
		&d3dDevice,               // returns the Direct3D device created
		&m_featureLevel,          // returns feature level of device created
		&d3dContext               // returns the device immediate context
		)
		);

	// Get the DirectX11.1 device by QI off the DirectX11 one.
	DX::ThrowIfFailed(d3dDevice.As(&m_d3dDevice));

	// And get the corresponding device context in the same way.
	DX::ThrowIfFailed(d3dContext.As(&m_d3dContext));

	// Obtain the underlying DXGI device of the Direct3D11.1 device.
	ComPtr<IDXGIDevice> dxgiDevice;
	DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

	if (m_imageSourceOrNull != NULL)
	{
		DX::ThrowIfFailed(D2D1CreateDevice(dxgiDevice.Get(), nullptr, &m_d2dDevice));
	}
	else
	{
		// Obtain the Direct2D device for 2D rendering.
		DX::ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
	}

	// And get its corresponding device context object.
	DX::ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext));

	// Save the DPI of this display in our class.
	m_d2dContext->SetDpi(m_dpi, m_dpi);

	// Release the swap chain (if it exists) as it will be incompatible with
	// the new device.
	m_swapChain = nullptr;

	if (m_imageSourceOrNull != NULL)
	{
		// Query for ISurfaceImageSourceNative interface.
		Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
		DX::ThrowIfFailed(m_imageSourceOrNull->QueryInterface(IID_PPV_ARGS(&sisNative)));

		// Associate the DXGI device with the SurfaceImageSource.
		DX::ThrowIfFailed(sisNative->SetDevice(dxgiDevice.Get()));
	}
}

// Allocate all memory resources that change on a window SizeChanged event.
void DXMain::CreateWindowSizeDependentResources()
{
	// Store the window bounds so the next time we get a SizeChanged event we can
	// avoid rebuilding everything if the size is identical.
	m_windowBounds = m_window->getBounds();

	//m_swapChain.Get().Release();
	outputln("Clean resources");

	this->cleanupSizeDependentResources();

	// If the swap chain already exists, resize it.
	if (m_swapChain != nullptr)
	{
		DX::ThrowIfFailed(m_swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
	}
	// Otherwise, create a new one.
	else
	{
		outputln("Create new swap chain");
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = DirectXHelper::getSwapChainDesc(DXGI_FORMAT_B8G8R8A8_UNORM);

		// Once the desired swap chain description is configured, it must be created on the same adapter as our D3D Device

		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory));

		// Create a swap chain for this window, using the DXGI factory.
		DX::ThrowIfFailed(m_window->createSwapChainForWindow(dxgiFactory.Get(), m_d3dDevice.Get(), &swapChainDesc, &m_swapChain));

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces 
		// latency and ensures that the application will only render after each VSync, minimizing power consumption.
		DX::ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBuffer));

	// Create a view interface on the rendertarget to use on bind.
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_renderTargetView));

	// Cache the rendertarget dimensions in our helper class for convenient use.
	D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
	m_backBuffer->GetDesc(&backBufferDesc);

	m_renderTargetSize.Width = static_cast<float>(backBufferDesc.Width);
	m_renderTargetSize.Height = static_cast<float>(backBufferDesc.Height);

	if (m_enableMultisampling)
	{
		m_multisampling = new Multisampling(this);
	}
	else
	{
		m_multisampling = NULL;

		// Create depth stencil buffer
		CD3D11_TEXTURE2D_DESC depthStencilDesc = DirectXHelper::getDepthStencilDesc(backBufferDesc.Width, backBufferDesc.Height, DirectXHelper::getSampleDescNo());
		ComPtr<ID3D11Texture2D> depthStencil;
		DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));

		// Create depth stencil state
		/*D3D11_DEPTH_STENCIL_DESC dsDesc = DirectXHelper::getDepthStencilDesc2();
		ComPtr<ID3D11DepthStencilState> depthStencilState;
		DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilState(&dsDesc, &depthStencilState));
		m_d3dContext->OMSetDepthStencilState(depthStencilState.Get(), 1);*/

		// Create depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = DirectXHelper::getDepthStencilViewDesc(depthStencilDesc, false);
		DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_depthStencilView));

		//m_d3dContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView.Get());
		//Assert(m_renderTargetView != NULL);

		//---------------------
		// set the current screen viewport

		m_screenViewport = DirectXHelper::getScreenViewport((float)m_renderTargetSize.Width, (float)m_renderTargetSize.Height);
		m_d3dContext->RSSetViewports(1, &m_screenViewport);
	}

	//---------------------
	// 2d

	// Now we set up the Direct2D render target bitmap linked to the swapchain. 
	// Whenever we render to this bitmap, it will be directly rendered to the 
	// swapchain associated with the window.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
		m_dpi,
		m_dpi
		);

	// Direct2D needs the dxgi version of the backbuffer surface pointer.
	ComPtr<IDXGISurface> dxgiBackBuffer;

	if (m_multisampling != NULL)
	{
		m_multisampling->affectDxgiBuffer(this, dxgiBackBuffer);
	}
	else
	{
		//CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D);
		//DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(overlayBuffer.Get(),&renderTargetViewDesc,&m_overlayRenderTargetView));
		DX::ThrowIfFailed(m_swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBackBuffer));
		//DX::ThrowIfFailed(m_backBuffer.As(&dxgiBuffer));
	}


	// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
	DX::ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, &m_d2dTargetBitmap));

	// So now we can set the Direct2D render target.
	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());

	// Set D2D text anti-alias mode to Grayscale to ensure proper rendering of text on intermediate surfaces.
	m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

// This routine is called in the event handler for the view SizeChanged event.
void DXMain::UpdateForWindowSizeChange()
{
	if (m_window->getBounds().Width != m_windowBounds.Width || m_window->getBounds().Height != m_windowBounds.Height)
	{
		outputln("Window size change (old=" << m_windowBounds.Width << "," << m_windowBounds.Height << ") new=(" << m_window->getBounds().Width << "," << m_window->getBounds().Height << ")");
		Utils::indentLog();
		m_d2dContext->SetTarget(nullptr);
		m_d2dTargetBitmap = nullptr;
		m_renderTargetView = nullptr;
		m_depthStencilView = nullptr;

		CreateWindowSizeDependentResources();
		Utils::unindentLog();

		m_imageSourceWidth = (int)m_window->getBounds().Width;
		m_imageSourceHeight = (int)m_window->getBounds().Height;
	}
}

// Helps track the DPI in the helper class.
// This is called in the dpiChanged event handler in the view class.
void DXMain::SetDpi(float dpi)
{
	if (dpi != m_dpi)
	{
		m_dpi = dpi;
		m_d2dContext->SetDpi(m_dpi, m_dpi);

		// Often a DPI change implies a window size change. In some cases Windows will issue
		// both a size changed event and a DPI changed event. In this case, the resulting bounds 
		// will not change, and the window resize code will only be executed once.
		UpdateForWindowSizeChange();
	}
}

// Method to deliver the final image to the display.
void DXMain::Present()
{
	if (m_multisampling != NULL) m_multisampling->resolve(this);

	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(1, 0);

	// If the device was removed either by a disconnect or a driver upgrade, we 
	// must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Initialize(m_window);
	}
	else
	{
		DX::ThrowIfFailed(hr);
	}

}

void DXMain::Trim()
{
#ifndef VERSION_WINDOWS_8_0
	ComPtr<IDXGIDevice3> dxgiDevice;
	m_d3dDevice.As(&dxgiDevice);
	dxgiDevice->Trim();
#endif
}

void DXMain::RunStepBegin()
{
	//long int msNow = Utils::getMillisecond();
	//CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
	if (m_imageSourceOrNull != NULL)
	{
		this->RunStepBeginForImageSource(m_imageSourceOrNull, m_imageSourceWidth, m_imageSourceHeight);
	}
	else
	{
		Assert(!m_isDrawing);
		m_isDrawing = true;
		m_d2dContext->BeginDraw();
		m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}

void DXMain::RunStepBeginForImageSource(IUnknown* imageSource, int width, int height)
{
	POINT offset;
	ComPtr<IDXGISurface> surface;

	// Express target area as a native RECT type.
	RECT updateRectNative;
	updateRectNative.left = 0;
	updateRectNative.top = 0;
	updateRectNative.right = width;
	updateRectNative.bottom = height;

	// Query for ISurfaceImageSourceNative interface.
	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(imageSource->QueryInterface(IID_PPV_ARGS(&sisNative)), L"imageSource->QueryInterface");

	// Begin drawing - returns a target surface and an offset to use as the top left origin when drawing.
	HRESULT beginDrawHR = sisNative->BeginDraw(updateRectNative, &surface, &offset);

	if (SUCCEEDED(beginDrawHR))
	{
		//------------------------
		// 3D

		// QI for target texture from DXGI surface.
		ComPtr<ID3D11Texture2D> d3DTexture;
		surface.As(&d3DTexture);

		// Create render target view.
		DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(d3DTexture.Get(), nullptr, &m_renderTargetView), L"m_d3dDevice->CreateRenderTargetView");

		// Set viewport to the target area in the surface, taking into account the offset returned by BeginDraw.
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = static_cast<float>(offset.x);
		viewport.TopLeftY = static_cast<float>(offset.y);
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = D3D11_MIN_DEPTH;
		viewport.MaxDepth = D3D11_MAX_DEPTH;
		m_d3dContext->RSSetViewports(1, &viewport);

		// Get the surface description in order to determine its size. The size of the depth/stencil buffer and the RenderTargetView must match, so the
		// depth/stencil buffer must be the same size as the surface. Since the whole surface returned by BeginDraw can potentially be much larger than
		// the actual update rect area passed into BeginDraw, it may be preferable for some apps to include an intermediate step which instead creates a
		// separate smaller D3D texture and renders into it before calling BeginDraw, then simply copies that texture into the surface returned by BeginDraw.
		// This would prevent needing to create a depth/stencil buffer which is potentially much larger than required, thereby saving memory at the cost of
		// additional overhead due to the copy operation.
		DXGI_SURFACE_DESC surfaceDesc;
		surface->GetDesc(&surfaceDesc);

		// Allocate a 2-D surface as the depth/stencil buffer.
		CD3D11_TEXTURE2D_DESC depthStencilDesc = DirectXHelper::getDepthStencilDesc(surfaceDesc.Width, surfaceDesc.Height, DirectXHelper::getSampleDescNo());
		ComPtr<ID3D11Texture2D> depthStencil;
		DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil), L"m_d3dDevice->CreateTexture2D");

		// Create depth/stencil view based on depth/stencil buffer.
		const CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D);
		DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_depthStencilView), L"m_d3dDevice->CreateDepthStencilView");

		//------------------------
		// 2D

		// Create render target.
		ComPtr<ID2D1Bitmap1> bitmap;
		DX::ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(surface.Get(), nullptr, &bitmap));

		// Set context's render target.
		m_d2dContext->SetTarget(bitmap.Get());
			
		// Begin drawing using D2D context.
		m_d2dContext->BeginDraw();
			
		// Apply a clip and transform to constrain updates to the target update area.
		// This is required to ensure coordinates within the target surface remain
		// consistent by taking into account the offset returned by BeginDraw, and
		// can also improve performance by optimizing the area that is drawn by D2D.
		// Apps should always account for the offset output parameter returned by 
		// BeginDraw, since it may not match the passed updateRect input parameter's location.
		m_d2dContext->PushAxisAlignedClip(
			D2D1::RectF(
			static_cast<float>(offset.x),
			static_cast<float>(offset.y),
			static_cast<float>(offset.x + width),
			static_cast<float>(offset.y + height)
			),
			D2D1_ANTIALIAS_MODE_ALIASED
			);

		m_d2dContext->SetTransform(D2D1::Matrix3x2F::Translation(static_cast<float>(offset.x), static_cast<float>(offset.y)));
	}
	else if (beginDrawHR == DXGI_ERROR_DEVICE_REMOVED || beginDrawHR == DXGI_ERROR_DEVICE_RESET)
	{
		// If the device has been removed or reset, attempt to recreate it and continue drawing.
		CreateDeviceResources();
		RunStepBeginForImageSource(imageSource, width, height);
	}
	else
	{
		// Notify the caller by throwing an exception if any other error was encountered.
		DX::ThrowIfFailed(beginDrawHR, L"beginDrawHR");
	}
}


void DXMain::RunStepEnd()
{
	if (m_imageSourceOrNull != NULL)
	{
		this->RunStepEndForImageSource(m_imageSourceOrNull);
	}
	else
	{
		HRESULT hr = m_d2dContext->EndDraw();
		m_isDrawing = false;

		if (hr == D2DERR_RECREATE_TARGET)
		{
			m_d2dContext->SetTarget(nullptr);
			m_d2dTargetBitmap = nullptr;
			CreateWindowSizeDependentResources();
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}

		//------------ present

		this->Present();
		//m_mainClass->m_frameDuration = Utils::getMillisecond() - 100;//msNow;
		//if (!m_mainClass->isRunning()) DirectXDraw::window->Close();
	}
}

void DXMain::RunStepEndForImageSource(IUnknown* imageSource)
{
	//------------------------
	// 2D

	// Remove the transform and clip applied in BeginDraw since
	// the target area can change on every update.
	m_d2dContext->SetTransform(D2D1::IdentityMatrix());
	m_d2dContext->PopAxisAlignedClip();
	// Remove the render target and end drawing.
	DX::ThrowIfFailed(m_d2dContext->EndDraw());

	m_d2dContext->SetTarget(nullptr);

	//------------------------
	// 2D & 3D

	// Query for ISurfaceImageSourceNative interface.
	Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
	DX::ThrowIfFailed(imageSource->QueryInterface(IID_PPV_ARGS(&sisNative)));

	DX::ThrowIfFailed(sisNative->EndDraw());
}

void DXMain::cleanupSizeDependentResources()
{
	if (m_multisampling != NULL)
	{
		delete m_multisampling;
		m_multisampling = NULL;
	}
	m_swapChain.Reset();
	m_depthStencilView.Reset();
	m_d2dTargetBitmap.Reset();
	m_backBuffer.Reset();
	m_offScreenSurface.Reset();
}

void DXMain::cleanup()
{
	this->cleanupSizeDependentResources();
	
	// http://blogs.msdn.com/b/chuckw/archive/2015/07/27/dxgi-debug-device.aspx
#ifndef NDEBUG
	Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
	HRESULT hr = m_d3dDevice.As(&d3dDebug);
	if (SUCCEEDED(hr))
	{
		Microsoft::WRL::ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		hr = d3dDebug.As(&d3dInfoQueue);
		if (SUCCEEDED(hr))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed 
			};
			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	outputln("m_d3dDevice: " << (void*)m_d3dDevice.Get());

	// http://stackoverflow.com/questions/13418176/manually-release-comptr

	m_d2dFactory.Reset();
#ifdef ENABLE_DIRECT_WRITE
	m_dwriteFactory.Reset();
#endif

	m_d2dDevice.Reset();
	m_d3dContext.Reset();
	m_d2dContext.Reset();
	m_d3dDevice.Reset();

	m_renderTargetView.Reset();

	m_d2dDevice = nullptr;
	m_d3dContext = nullptr;
	m_d2dContext = nullptr;
	m_d3dDevice = nullptr;
}

DXMain::~DXMain()
{

}

void DXMain::uninitializeWindow()
{
	//m_window->Close();
	if (m_window != NULL)
	{
		delete m_window;
		m_window = NULL;
	}
}

// Clears the surface with the given color.  This must be called 
// after BeginDraw and before EndDraw for a given update.
void DXMain::Clear(const Color& color)
{
	// Convert color values.
	const float clearColor[4] = { color.r() / 255.0f, color.g() / 255.0f, color.b() / 255.0f, color.a() / 255.0f };
	// Clear render target view with given color.
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

	D2D1::ColorF colorDx(color.r() / 255.f, color.g() / 255.f, color.b() / 255.f, color.a() / 255.f);
	m_d2dContext->Clear(colorDx);
}

void DXMain::OnSuspending()
{
	ComPtr<IDXGIDevice3> dxgiDevice;
	m_d3dDevice.As(&dxgiDevice);

	// Hints to the driver that the app is entering an idle state and that its memory can be used temporarily for other apps.
	// If not done, it might raise an: ACK error: One or more applications in the package did not call Trim() on their DXGI Devices while being suspended.
	dxgiDevice->Trim();
}

DXMain* DXMain::instance()
{
	return s_instance;
}

//-------------------------------------------------------------------------

void DXMain::drawBitmap(
	ID2D1Bitmap* bitmap, const Float2& position, const Float2& scale, float opacity, D2D1_RECT_F* rectSource, D2D1_RECT_F* rectDest, float angleDegree, const Int2& rotationCenterPos,
	bool horizontalMirror, bool verticalMirror)
{
	Float2 zoomRatio(m_virtualPos.x() / m_ppp.x(), m_virtualPos.y() / m_ppp.y());
	DirectXDraw::drawBitmap(m_d2dContext.Get(), zoomRatio, m_defaultDpi, bitmap, position, scale, opacity, rectSource, rectDest, angleDegree, rotationCenterPos, horizontalMirror, verticalMirror);
}
void DXMain::drawRoundedRectangle(const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, float round, float borderSize, bool fill)
{
	Float2 zoomRatio(m_virtualPos.x() / m_ppp.x(), m_virtualPos.y() / m_ppp.y());
	DirectXDraw::drawRoundedRectangle(m_d2dContext.Get(), zoomRatio, posLeftTop, posRightBottom, color, round, borderSize, fill);
}
void DXMain::drawLine(const Int2& pos1, const Int2& pos2, const Color& color, float thickness)
{
	Float2 zoomRatio(m_virtualPos.x() / m_ppp.x(), m_virtualPos.y() / m_ppp.y());
	DirectXDraw::drawLine(m_d2dContext.Get(), zoomRatio, pos1, pos2, color, thickness);
}
ID2D1Bitmap* DXMain::createBitmap(const D2D1_SIZE_U& sizeBitmap)
{
	return DirectXDraw::createBitmap(m_d2dContext.Get(), sizeBitmap);
}

//-------------------------------------------------------------------------

#ifdef ENABLE_DIRECT_WRITE

void DXMain::drawTextLayout(ID2D1SolidColorBrush* solidBrush, IDWriteTextLayout* textLayout, const Int2& position, const Float2& scale)
{
	Float2 zoomRatio(m_virtualPos.x() / m_ppp.x(), m_virtualPos.y() / m_ppp.y());
	DirectXDraw::drawTextLayout(m_d2dContext.Get(), zoomRatio, solidBrush, textLayout, position, scale);
}
IDWriteTextFormat* DXMain::createTextFormat(const char* fontName, bool alignmentCenterX, bool alignmentCenterY)
{
	return  DirectXDraw::createTextFormat(m_dwriteFactory.Get(), fontName, alignmentCenterX, alignmentCenterY);
}
ID2D1SolidColorBrush* DXMain::createSolidColorBrush(const Color& color)
{
	return DirectXDraw::createSolidColorBrush(m_d2dContext.Get(), color);
}
IDWriteTextLayout* DXMain::createTextLayout(IDWriteTextFormat* textFormat, const Int2& layoutSize, float scale, const wchar_t* wtext)
{
	return DirectXDraw::createTextLayout(m_dwriteFactory.Get(), textFormat, layoutSize, scale, wtext);
}
#endif

//-------------------------------------------------------------------------

void DXMain::initializeDpi() {
	if (m_defaultDpi == Float2(-1.f, 1.f))
		m_d2dContext->GetDpi(&(m_defaultDpi.data[0]), &(m_defaultDpi.data[1]));
}

void DXMain::getResolution(int* horizontal, int* vertical)
{
	ComPtr<IDXGIDevice> dxgiDevice;
	DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

	ComPtr<IDXGIAdapter> dxgiAdapter;
	DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

	IDXGIOutput * pOutput;
	if (dxgiAdapter->EnumOutputs(0, &pOutput) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		pOutput->GetDesc(&desc);
		*horizontal = desc.DesktopCoordinates.right;
		*vertical = desc.DesktopCoordinates.bottom;
	}
}

void DXMain::flushContext()
{
	m_d2dContext->Flush();
}

void DXMain::setPixelPerPoint(const Float2& ppp, const Int2& topLeftCornerInWindow)
{
	m_ppp = ppp;
	m_virtualPos = topLeftCornerInWindow;
	this->set2DMode();
}

void DXMain::set2DMode()
{
	m_d2dContext->SetDpi(m_defaultDpi.x() * m_ppp.x(), m_defaultDpi.y() * m_ppp.y());
}

void DXMain::set3DMode()
{
	m_d2dContext->SetDpi(m_defaultDpi.x(), m_defaultDpi.y());
}

const Int2& DXMain::getTopLeftCornerInWindow()
{
	return m_virtualPos;
}

const Float2& DXMain::getPPP()
{
	return m_ppp;
}

//-------------------------------------------------------------------------
