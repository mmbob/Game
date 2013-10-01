#include <WindowsX.h>
#include <cassert>

#include "DirectX.h"

template <typename T>
void SafeRelease(T* p)
{
	if (p != nullptr)
		p->Release();
}

void DirectXManager::Init()
{
	pDirectX = Direct3DCreate9(DIRECT3D_VERSION);
	HRESULT result = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &pInput, nullptr);

	assert(pDirectX != nullptr);
	assert(SUCCEEDED(result));

	pInput->CreateDevice(GUID_SysMouse, &pMouse, nullptr);
	pInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, nullptr);
}

void DirectXManager::UnInit()
{
	SafeRelease(pMouse);
	SafeRelease(pKeyboard);
	SafeRelease(pInput);
	SafeRelease(pDirectX);
}

void DirectXManager::InitDeviceObjects(HWND window, int width, int height)
{
	D3DPRESENT_PARAMETERS parameters;
	ZeroMemory(&parameters, sizeof(parameters));
	parameters.Windowed = true;
	parameters.hDeviceWindow = window;
	parameters.BackBufferCount = 1;
	parameters.BackBufferFormat = D3DFMT_UNKNOWN;
	parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.EnableAutoDepthStencil = true;
	parameters.AutoDepthStencilFormat = D3DFMT_D16;

	HRESULT hr = pDirectX->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &pDevice);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = D3DXCreateSprite(pDevice, &pSprite);
	if (FAILED(hr))
		DebugBreak();	// fix


	hr = pMouse->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = pMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = pKeyboard->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		DebugBreak();	// fix

	D3DXMATRIX World;
	D3DXMatrixIdentity(&World);
	pDevice->SetTransform(D3DTS_WORLD, &World);

	D3DXMATRIX ProjMatrix;
	D3DXMatrixPerspectiveFovLH(&ProjMatrix, D3DX_PI / 4, float(width) / float(height), 0.0f, 100.0f);
	pDevice->SetTransform(D3DTS_PROJECTION, &ProjMatrix);

	D3DXMATRIX ViewMatrix;
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtLH(&ViewMatrix, &vEyePt, &vLookatPt, &vUpVec);
	pDevice->SetTransform(D3DTS_VIEW, &ViewMatrix);
}

void DirectXManager::ReleaseDeviceObjects()
{
	SafeRelease(pSprite);

	SafeRelease(pDevice);
}

LPDIRECT3DDEVICE9 DirectXManager::GetDevice() const
{
	return pDevice;
}

LPD3DXSPRITE DirectXManager::GetSprite() const
{
	return pSprite;
}