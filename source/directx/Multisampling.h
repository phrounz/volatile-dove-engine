#ifndef Multisampling_h_INCLUDED
#define Multisampling_h_INCLUDED

//#if defined(USES_WINDOWS8_DESKTOP)

#include "directx_inc.h"

#include "../../include/CoreUtils.h"

#include "DXMain.h"

class Multisampling
{
public:
	static bool checkSupport(Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice);
	static DXGI_SAMPLE_DESC getSampleDescMultisampled(Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice);

	Multisampling(DXMain* directXBase);
	void affectDxgiBuffer(DXMain* directXBase, Microsoft::WRL::ComPtr<IDXGISurface>& dxgiBuffer);
	void resolve(DXMain* directXBase);
	~Multisampling() {}
private:
	void setMultisampling(DXMain* directXBase);
};

//#endif

#endif //Multisampling_h_INCLUDED
