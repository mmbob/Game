#include "Renderer.h"

#include <cassert>
#include <algorithm>

void Renderer::Init(HWND window)
{
	pDirectX = new DirectXManager;
	pDirectX->Init();
	pDirectX->InitDeviceObjects(window, 800, 600);

	HDC context = GetDC(window);
	const int SanityFontHeight = -MulDiv(32, GetDeviceCaps(context, LOGPIXELSY), 72);
	const int DayTimeFontHeight = -MulDiv(24, GetDeviceCaps(context, LOGPIXELSY), 72);
	ReleaseDC(window, context);

	D3DXFONT_DESC desc;
	desc.Height = SanityFontHeight;
	desc.Width = 0;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.CharSet = DEFAULT_CHARSET;
	desc.Italic = false;
	desc.Weight = FW_ULTRALIGHT;
	desc.PitchAndFamily = DEFAULT_PITCH || FF_DONTCARE;
	desc.Quality = DEFAULT_QUALITY;
	wcscpy_s(desc.FaceName, L"Tahoma");

	HRESULT result = D3DXCreateFontIndirect(pDirectX->GetDevice(), &desc, &pSanityFont);
	assert(SUCCEEDED(result));

	desc.Height = DayTimeFontHeight;

	result = D3DXCreateFontIndirect(pDirectX->GetDevice(), &desc, &pDayTimeFont);
	assert(SUCCEEDED(result));

	pSanityFont->PreloadCharacters('0', '9');
	pSanityFont->PreloadCharacters('-', '-');
	pSanityFont->PreloadCharacters('+', '+');

	pDayTimeFont->PreloadCharacters('0', '9');
	pDayTimeFont->PreloadCharacters('a', 'a');
	pDayTimeFont->PreloadCharacters('p', 'p');
	pDayTimeFont->PreloadCharacters('m', 'm');
	pDayTimeFont->PreloadCharacters(':', ':');

	sanityText = L"0099";
	sanityUpdateText = L"+05";
	dayText = L"152";
	timeText = L"11:55am";

	SetRectEmpty(&sanityRect);
	SetRectEmpty(&sanityUpdateRect);
	SetRectEmpty(&dayRect);
	SetRectEmpty(&timeRect);

	pSanityFont->DrawText(nullptr, L"0000", -1, &sanityRect, DT_CALCRECT | DT_SINGLELINE, 0);
	pSanityFont->DrawText(nullptr, L"+99", -1, &sanityUpdateRect, DT_CALCRECT | DT_SINGLELINE, 0);
	pDayTimeFont->DrawText(nullptr, L"999", -1, &dayRect, DT_CALCRECT | DT_SINGLELINE, 0);
	pDayTimeFont->DrawText(nullptr, L"12:00am", -1, &timeRect, DT_CALCRECT | DT_SINGLELINE, 0);

	D3DVIEWPORT9 viewport;
	pDirectX->GetDevice()->GetViewport(&viewport);

	SetRect(&sanityRect, 60, viewport.Height - sanityRect.bottom - 10, 60 + sanityRect.right, viewport.Height - 10);
	SetRect(&sanityUpdateRect, sanityRect.right, sanityRect.top, sanityRect.right + sanityUpdateRect.right, sanityRect.bottom);
	SetRect(&timeRect, viewport.Width - timeRect.right - 10, viewport.Height - timeRect.bottom - 10, viewport.Width - 10, viewport.Height - 10);
	SetRect(&dayRect, timeRect.left - dayRect.right - 10, timeRect.top, timeRect.left - 10, timeRect.bottom);

	D3DXCreateTextureFromFile(pDirectX->GetDevice(), L"Resources\\Sprites\\Background.png", &pBackgroundTexture);
	D3DXCreateTextureFromFile(pDirectX->GetDevice(), L"Resources\\Sprites\\Sanity.png", &pSanityTexture);
}

void Renderer::UnInit()
{
	pSanityFont->Release();
	pDayTimeFont->Release();
}

void Renderer::RenderUI()
{
	LPD3DXSPRITE pSprite = pDirectX->GetSprite();
	pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXMATRIX oldTransform;
	pSprite->GetTransform(&oldTransform);

	D3DXMATRIX transform;
	D3DXMatrixIdentity(&transform);
	D3DXMatrixTransformation2D(&transform, nullptr, 0.0f, nullptr, nullptr, 0.0f, &D3DXVECTOR2(0, 500));

	pSprite->SetTransform(&transform);
	pSprite->Draw(pBackgroundTexture, nullptr, nullptr, nullptr, 0xFFFFFFFF);

	D3DXMatrixIdentity(&transform);
	D3DXMatrixTransformation2D(&transform, nullptr, 0.0f, nullptr, nullptr, 0.0f, &D3DXVECTOR2(10, 520));

	pSprite->SetTransform(&transform);
	pSprite->Draw(pSanityTexture, nullptr, nullptr, nullptr, 0xFFFFFFFF);

	pSprite->SetTransform(&oldTransform);

	static int alpha = 255;
	alpha -= 2;
	if (alpha <= 0)
		alpha = 255;

	RECT transformedRect = sanityUpdateRect;

	pSanityFont->DrawText(pSprite, sanityText.c_str(), -1, &sanityRect, DT_SINGLELINE, 0xFFFFFFFF);
	pSanityFont->DrawText(pSprite, sanityUpdateText.c_str(), -1, &transformedRect, DT_SINGLELINE, D3DCOLOR_ARGB(alpha, 255, 255, 255));
	pDayTimeFont->DrawText(pSprite, dayText.c_str(), -1, &dayRect, DT_SINGLELINE, 0xFFFFFFFF);
	pDayTimeFont->DrawText(pSprite, timeText.c_str(), -1, &timeRect, DT_SINGLELINE, 0xFFFFFFFF);

	pSprite->End();
}

void Renderer::RenderWorld()
{
	
}

void Renderer::Render()
{
	LPDIRECT3DDEVICE9 pDevice = pDirectX->GetDevice();

	HRESULT result = pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	if (SUCCEEDED(result = pDevice->BeginScene()))
	{
		RenderWorld();
		RenderUI();

		assert(SUCCEEDED(pDevice->EndScene()));
		pDevice->Present(nullptr, nullptr, nullptr, nullptr);
	}
	else
		DebugBreak();
}