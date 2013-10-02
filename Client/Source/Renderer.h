#pragma once

#include <string>
#include <unordered_map>

#include "DirectX.h"

using namespace std;

class Renderer
{
	DirectXManager* pDirectX;

	unordered_map<wstring, LPDIRECT3DTEXTURE9> textureMap;

	LPD3DXFONT pSanityFont;
	LPD3DXFONT pDayTimeFont;

	wstring sanityText;
	wstring sanityUpdateText;
	wstring dayText;
	wstring timeText;

	RECT sanityRect;
	RECT sanityUpdateRect;
	RECT dayRect;
	RECT timeRect;

	void LoadTextures();

	void RenderUI() const;
	void RenderWorld() const;
public:
	void Init(HWND window);
	void UnInit();

	LPDIRECT3DTEXTURE9 GetTexture(wstring textureName) const;

	void Render() const;
};