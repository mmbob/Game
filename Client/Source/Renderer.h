#pragma once

#include <string>
#include <unordered_map>
#include <list>

#include "DirectX.h"
#include "RenderObject.h"

using namespace std;

class Renderer
{
	DirectXManager* pDirectX;

	list<IRenderObject*> objects;
	unordered_map<wstring, LPDIRECT3DTEXTURE9> textureMap;

	D3DXVECTOR2 cameraPosition;

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
	void RenderTiles() const;
public:
	void Init(DirectXManager* pDirectX, HWND window);
	void UnInit();

	bool AddRenderObject(IRenderObject* pObject);

	LPDIRECT3DTEXTURE9 GetTexture(wstring textureName) const;

	void Update();
	void Render() const;
};