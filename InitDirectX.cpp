#include <WindowsX.h>

#include "InitDirectX.h"

template <typename T>
void SafeRelease(T* p)
{
	if (p != nullptr)
		p->Release();
}

void DirectXManager::Init()
{
	pDirectX = Direct3DCreate9(DIRECT3D_VERSION);
}

void DirectXManager::InitDeviceObjects(HWND window, int width, int height)
{
	D3DPRESENT_PARAMETERS parameters;
	ZeroMemory(&parameters, sizeof(parameters));
	parameters.Windowed = true;
	parameters.hDeviceWindow = window;
	parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferWidth = 800;
	parameters.BackBufferHeight = 600;
	parameters.BackBufferFormat = D3DFMT_R8G8B8;
	parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	parameters.SwapEffect = D3DSWAPEFFECT_FLIP;

	HRESULT hr = pDirectX->CreateDevice(0, D3DDEVTYPE_HAL, nullptr, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &pDevice);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = D3DXCreateSprite(pDevice, &pSprite);
	if (FAILED(hr))
		DebugBreak();	// fix

	HDC context = GetDC(window);
	hr = D3DXCreateFont(pDevice, -MulDiv(11, GetDeviceCaps(context, LOGPIXELSY), 72), 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, 0, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Tahoma", &pFont);
	ReleaseDC(window, context);
	if (FAILED(hr))
		DebugBreak();	// fix

	
}

void DirectXManager::ReleaseDeviceObjects()
{
	SafeRelease(pFont);
	SafeRelease(pSprite);

	SafeRelease(pDevice);
}