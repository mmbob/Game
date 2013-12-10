#pragma once

#include <Box2D/Box2D.h>
#include <string>
#include <unordered_map>
#include <list>

#include "DirectX.h"
#include "RenderObject.h"
#include "GameWorld.h"

using namespace std;

class PhysicsDebugDraw;

class Renderer
{
	static const int PixelsPerTile = 64;

	DirectXManager* pDirectX;

	list<IRenderObject*> objects;
	list<IRenderObject*> uiObjects;
	bool hasObjectsChanged;
	unordered_map<wstring, LPDIRECT3DTEXTURE9> textureMap;
	std::unordered_map<std::wstring, LPD3DXFONT> fontMap;

	D3DXVECTOR2 cameraPosition;
	Rect gameArea;
	D3DCOLOR ambientColor;

	D3DXMATRIX worldToScreenTransform;
	D3DXMATRIX screenToWorldTransform;

	GameWorld* gameWorld;

	b2World* physics;
	PhysicsDebugDraw* debugDraw;

	void UpdateTranforms();

	void LoadTextures();

	void RenderObjectList(const list<IRenderObject*>& list, const D3DXMATRIX& positionTransform) const;
	void RenderWorld() const;
	void RenderTiles(const D3DXMATRIX& positionTransform) const;
public:
	void Init(DirectXManager* pDirectX, HWND window);
	void UnInit();

	bool AddRenderObject(IRenderObject* pObject);
	bool AddUIObject(IRenderObject* pObject);

	bool RemoveRenderObject(IRenderObject* pObject);
	bool RemoveUIObject(IRenderObject* pObject);

	LPDIRECT3DTEXTURE9 GetTexture(wstring textureName) const;

	const D3DXVECTOR2& GetCameraPosition() const;
	void SetCameraPosition(const D3DXVECTOR2& position);

	const RECT& GetGameArea() const;
	void SetGameArea(const RECT& gameArea);

	const GameWorld* GetGameWorld() const;
	void SetGameWorld(GameWorld* gameWorld);

	void SetAmbientColor(D3DCOLOR ambientColor);

	bool WorldToScreen(const D3DXVECTOR3& world, D3DXVECTOR2* pScreen) const;
	bool ScreenToWorld(const D3DXVECTOR2& screen, D3DXVECTOR3* pWorld) const;

	float PixelsToGameUnits(int pixels) const;
	int GameUnitsToPixels(float units) const;

	void Update();
	void Render() const;

	DirectXManager* GetDirectX() const;

	void AddFont(const std::wstring& name, LPD3DXFONT font);
	void RemoveFont(const std::wstring& name);
	LPD3DXFONT GetFont(const std::wstring& name);

	void SetPhysicsDraw(b2World* physics, PhysicsDebugDraw* debugDraw);
};