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
	list<IRenderObject*> uiObjects;
	bool hasObjectsChanged;
	unordered_map<wstring, LPDIRECT3DTEXTURE9> textureMap;

	D3DXVECTOR2 cameraPosition;
	Rect gameArea;
	D3DCOLOR ambientColor;

	void LoadTextures();

	void RenderObjectList(const list<IRenderObject*>& list) const;
	void RenderUI() const;
	void RenderWorld() const;
	void RenderTiles() const;
public:
	void Init(DirectXManager* pDirectX, HWND window);
	void UnInit();

	bool AddRenderObject(IRenderObject* pObject);
	bool AddUIObject(IRenderObject* pObject);

	LPDIRECT3DTEXTURE9 GetTexture(wstring textureName) const;

	const D3DXVECTOR2& GetCameraPosition() const;
	void SetCameraPosition(const D3DXVECTOR2& position);

	const RECT& GetGameArea() const;
	void SetGameArea(const RECT& gameArea);

	void SetAmbientColor(D3DCOLOR ambientColor);

	bool WorldToScreen(const D3DXVECTOR3& world, D3DXVECTOR2* pScreen) const;
	bool ScreenToWorld(const D3DXVECTOR2& screen, D3DXVECTOR3* pWorld) const;

	void Update();
	void Render() const;
};