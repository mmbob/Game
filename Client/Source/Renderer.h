#pragma once

#include <string>

#include "DirectX.h"

using namespace std;

class Renderer
{
	DirectXManager* pDirectX;

	LPD3DXFONT pSanityFont;
	LPD3DXFONT pDayTimeFont;

	LPDIRECT3DTEXTURE9 pBackgroundTexture;
	LPDIRECT3DTEXTURE9 pSanityTexture;

	wstring sanityText;
	wstring sanityUpdateText;
	wstring dayText;
	wstring timeText;

	RECT sanityRect;
	RECT sanityUpdateRect;
	RECT dayRect;
	RECT timeRect;

	void RenderUI();
	void RenderWorld();
public:
	void Init(HWND window);
	void UnInit();

	void Render();
};