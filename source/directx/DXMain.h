#ifndef DirectXMainObjects_h_INCLUDED
#define DirectXMainObjects_h_INCLUDED

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

//----------------------------------------

#include "../../include/CoreUtils.h"

#include "DirectXHelper.h"

class Multisampling;
class AbstractWindow;

namespace ScenarioComponent
{
	ref class ScenarioImageSource;
}


//----------------------------------------

class DXMain
{
	//friend class Scene2D;
	//friend class Shader;
	//friend class TextureReal;
	//friend class VBO;
	friend class Multisampling;
	friend class Scene3DPrivate;
	friend class RenderToTexture;

	friend ref class ScenarioComponent::ScenarioImageSource;
	
public:
	DXMain(IUnknown* imageSourceOrNull = NULL, int imageSourceWidth = -1, int imageSourceHeight = -1);

	//-------------------
	// initialization and events-related methods

	// These are the resources required independent of hardware.
	void Initialize(AbstractWindow* window);
	void getWindowSize(int* width, int* height);
	void CreateDeviceIndependentResources();
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void UpdateForWindowSizeChange();
	void SetDpi(float dpi);
	void Present();
	void Trim();
	void RunStepBegin();
	void RunStepEnd();
	void cleanupSizeDependentResources();
	void cleanup();
	~DXMain();
	void uninitializeWindow();
	void initializeDpi();

	//-------------------
	// misc

	void SetMultisampling(bool yesNo) { m_enableMultisampling = yesNo; }

	const AbstractWindow* getWindow() const { return m_window; }

	void Clear(const Color& color);

	void OnSuspending();

	void flushContext();

	void setPixelPerPoint(const Float2& ppp, const Int2& topLeftCornerInWindow);
	void set2DMode();
	void set3DMode();
	const Int2& getTopLeftCornerInWindow();
	const Float2& getPPP();

	//-------------------
	// static accessors

	static ID2D1DeviceContext* get2DContext() { return instance()->m_d2dContext.Get(); }
	static ID3D11Device1* getDevice() { return instance()->m_d3dDevice.Get(); }
	static ID3D11DeviceContext1* getContext() { return instance()->m_d3dContext.Get(); }
#ifdef ENABLE_DIRECT_WRITE
	static IDWriteFactory1* getWriteFactory() { return instance()->m_dwriteFactory.Get(); }
#endif
	static DXMain* instance();

	//-------------------
	// drawing methods

	void drawBitmap(
		ID2D1Bitmap* bitmap, const Float2& position, const Float2& scale, float opacity, D2D1_RECT_F* rectSource, D2D1_RECT_F* rectDest, float angleDegree, const Int2& rotationCenterPos,
		bool horizontalMirror, bool verticalMirror);
	void drawRoundedRectangle(const Int2& posLeftTop, const Int2& posRightBottom, const Color& color, float round, float borderSize, bool fill);
	void drawLine(const Int2& pos1, const Int2& pos2, const Color& color, float thickness);
	ID2D1Bitmap* createBitmap(const D2D1_SIZE_U& sizeBitmap);

#ifdef ENABLE_DIRECT_WRITE
	void drawTextLayout(ID2D1SolidColorBrush* solidBrush, IDWriteTextLayout* textLayout, const Int2& position, const Float2& scale);
	IDWriteTextFormat* createTextFormat(const char* fontName, bool alignmentCenterX, bool alignmentCenterY);
	ID2D1SolidColorBrush* createSolidColorBrush(const Color& color);
	IDWriteTextLayout* createTextLayout(IDWriteTextFormat* textFormat, const Int2& layoutSize, float scale, const wchar_t* wtext);
#endif

	bool isDrawing() const { return m_isDrawing; }
	
private:

	// Direct2D Objects
	Microsoft::WRL::ComPtr<ID2D1Factory1>          m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>            m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>     m_d2dContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>           m_d2dTargetBitmap;

#ifdef ENABLE_DIRECT_WRITE
	// DirectWrite & Windows Imaging Component Objects
	Microsoft::WRL::ComPtr<IDWriteFactory1>        m_dwriteFactory;
#endif

	// Direct3D Objects
	Microsoft::WRL::ComPtr<ID3D11Device1>          m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>   m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	D3D_FEATURE_LEVEL                              m_featureLevel;
	Windows::Foundation::Size                      m_renderTargetSize;
	Windows::Foundation::Rect                      m_windowBounds;

	AbstractWindow*                                m_window;
	//Microsoft::WRL::ComPtr<Windows::UI::Core::CoreWindow> m_window;
	//Platform::Agile<Windows::UI::Core::CoreWindow^> m_window;
	float                                          m_dpi;


	D3D11_VIEWPORT									m_screenViewport;
	
	//float        m_scalingFactor;
	//MultisamplingSupportInfo^ m_supportInfo;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_backBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_offScreenSurface;// for multisampling

	Multisampling* m_multisampling;

	bool m_isDrawing;
	bool m_enableMultisampling;

	IUnknown* m_imageSourceOrNull;
	int m_imageSourceWidth;
	int m_imageSourceHeight;
	void RunStepBeginForImageSource(IUnknown* imageSourceOrNull, int width, int height);
	void RunStepEndForImageSource(IUnknown* imageSource);

	Float2 m_defaultDpi;
	Int2 m_virtualPos;
	Float2 m_ppp;

	static DXMain* s_instance;
};

#endif //DirectXMainObjects_h_INCLUDED
