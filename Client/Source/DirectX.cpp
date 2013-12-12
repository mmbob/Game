#include <WindowsX.h>
#include <cassert>

#include "DirectX.h"
#include "Util.h"

void DirectXManager::Init()
{
	pDirectX = Direct3DCreate9(DIRECT3D_VERSION);
	HRESULT result = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &pInput, nullptr);

	assert(pDirectX != nullptr);
	assert(SUCCEEDED(result));

	result = pInput->CreateDevice(GUID_SysMouse, &pMouse, nullptr);
	assert(SUCCEEDED(result));
	result = pInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, nullptr);
	assert(SUCCEEDED(result));
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
	parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.EnableAutoDepthStencil = true;
	parameters.AutoDepthStencilFormat = D3DFMT_D16;

	HRESULT hr = pDirectX->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &pDevice);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = D3DXCreateSprite(pDevice, &pSprite);
	if (FAILED(hr))
		DebugBreak();	// fix

	hr = D3DXCreateLine(pDevice, &pLine);
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

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	pDevice->SetTransform(D3DTS_WORLD, &world);

	D3DXMATRIX projection;
	D3DXMatrixPerspectiveFovLH(&projection, D3DX_PI / 4, float(width) / float(height), 0.0f, 100.0f);
	pDevice->SetTransform(D3DTS_PROJECTION, &projection);

	D3DXMATRIX view;
	D3DXVECTOR3 eyePoint(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lookAtPoint(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 upVector(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtLH(&view, &eyePoint, &lookAtPoint, &upVector);
	pDevice->SetTransform(D3DTS_VIEW, &view);
}

void DirectXManager::ReleaseDeviceObjects()
{
	pKeyboard->Unacquire();
	pMouse->Unacquire();

	SafeRelease(pLine);
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

LPD3DXLINE DirectXManager::GetLine() const
{
	return pLine;
}

void DirectXManager::Update()
{
	HRESULT result;

	result = pKeyboard->Poll();
	if (FAILED(result))
	{
		while (pKeyboard->Acquire() == DIERR_INPUTLOST);

		ZeroMemory(keyState, sizeof(keyState));
		return;
	}
	result = pKeyboard->GetDeviceState(sizeof(keyState), keyState);
	assert(SUCCEEDED(result));

	int index = 0;
	for (unsigned char key : keyState)
	{
		if ((key & 0x80) == 0)
			keyPressedTime[index] = 0;
		index++;
	}

	result = pMouse->Poll();
	if (FAILED(result))
	{
		while (pMouse->Acquire() == DIERR_INPUTLOST);

		ZeroMemory(&mouseState, sizeof(mouseState));
		return;
	}
	result = pMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	assert(SUCCEEDED(result));
}

bool DirectXManager::IsKeyDown(int keyCode) const
{
	if ((keyState[keyCode] & 0x80) > 0)
		return true;
	return false;
}

bool DirectXManager::IsKeyPressed(int keyCode) const
{
	const DWORD KeyDelay = 200;
	DWORD currentTime = GetTickCount();

	if ((keyState[keyCode] & 0x80) > 0 && (currentTime - keyPressedTime[keyCode]) > KeyDelay)
	{
		keyPressedTime[keyCode] = currentTime;

		return true;
	}
	return false;
}

int DirectXManager::GetMouseX() const
{
	return mouseState.lX;
}

int DirectXManager::GetMouseY() const
{
	return mouseState.lY;
}

int DirectXManager::GetMouseWheel() const
{
	return mouseState.lZ;
}

bool DirectXManager::IsMousePressed(MouseButton button) const
{
	switch (button)
	{
	case MB_Left:
		return (mouseState.rgbButtons[0] & 0x80) > 0;
		break;

	case MB_Right:
		return (mouseState.rgbButtons[1] & 0x80) > 0;
		break;

	case MB_Middle:
		return (mouseState.rgbButtons[2] & 0x80) > 0;
		break;

	case MB_4:
		return (mouseState.rgbButtons[3] & 0x80 ) > 0;
		break;

	default:
		return false;
	}
}