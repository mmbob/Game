#include "Renderer.h"

#include <array>
#include <cassert>
#include <algorithm>

#include "GameWorld.h"

void Renderer::Init(DirectXManager* pDirectX, HWND)
{
	this->pDirectX = pDirectX;
	debugDraw = nullptr;
	ambientColor = 0xFFFFFFFF;

	LoadTextures();
}

void Renderer::UnInit()
{
}

void Renderer::LoadTextures()
{
	WIN32_FIND_DATA findData;
	HANDLE find = FindFirstFile(L"Resources\\Sprites\\*.png", &findData);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			wstring filePath(L"Resources\\Sprites\\");
			filePath += findData.cFileName;
			wstring fileNameNoExtension(findData.cFileName);
			fileNameNoExtension = fileNameNoExtension.substr(0, fileNameNoExtension.rfind('.'));

			LPDIRECT3DTEXTURE9 pTexture;
			HRESULT result = D3DXCreateTextureFromFile(pDirectX->GetDevice(), filePath.c_str(), &pTexture);
			if (SUCCEEDED(result))
			{
				textureMap[fileNameNoExtension] = pTexture;
			}
		} while (FindNextFile(find, &findData));
	}
}

LPDIRECT3DTEXTURE9 Renderer::GetTexture(wstring textureName) const
{
	auto iter = textureMap.find(textureName);
	if (iter == textureMap.end())
		return nullptr;
	return iter->second;
}

void Renderer::RenderTiles(const D3DXMATRIX& positionTransform) const
{
	auto pSprite = pDirectX->GetSprite();

	LPDIRECT3DTEXTURE9 tileSets[] =
	{
		GetTexture(L"Tiles1"),
	};

	D3DXVECTOR2 screenUpperLeft(float(gameArea.left), float(gameArea.top));
	D3DXVECTOR3 worldUpperLeft;
	ScreenToWorld(screenUpperLeft, &worldUpperLeft);

	D3DXVECTOR2 screenLowerRight(float(gameArea.right), float(gameArea.bottom));
	D3DXVECTOR3 worldLowerRight;
	ScreenToWorld(screenLowerRight, &worldLowerRight);

	for (int layer = 0; layer < 2; ++layer)
	{
		for (int x = int(worldUpperLeft.x); x <= int(worldLowerRight.x + 1); ++x)
		{
			for (int y = int(worldUpperLeft.y); y <= int(worldLowerRight.y + 1); ++y)
			{
				auto tile = gameWorld->GetTile(layer, x, y);

				D3DXVECTOR3 rawPosition(float(x - 0.5f), float(y - 0.5f), 1.0f);
				D3DXVECTOR4 position4;
				D3DXVec3Transform(&position4, &rawPosition, &positionTransform);

				D3DXVECTOR3 position3(position4.x, position4.y, rawPosition.z);
				pSprite->Draw(tileSets[tile.Tileset], &tile.TextureClip, nullptr, &position3, ambientColor);
}
		}
	}
}

void Renderer::RenderObjectList(const list<IRenderObject*>& list, const D3DXMATRIX& positionTransform) const
{
	auto pSprite = pDirectX->GetSprite();

	for (IRenderObject* object : list)
	{
		switch (object->GetType())
		{
		case RenderObjectType::Texture:
		{
			auto texObject = reinterpret_cast<TextureRenderObject*>(object);

			std::wstring name;
			texObject->GetTextureName(&name);

			RECT clip;
			texObject->GetTextureClip(&clip);

			D3DXMATRIX rotation;
			texObject->GetRotation(&rotation);

			D3DXVECTOR3 rawPosition;
			texObject->GetPosition(&rawPosition);

			D3DXVECTOR4 position4;
			D3DXVec3Transform(&position4, &rawPosition, &positionTransform);

			D3DCOLOR color;
			texObject->GetColor(&color);

			pSprite->SetTransform(&rotation);
			D3DXVECTOR3 position3(position4.x, position4.y, rawPosition.z);
			pSprite->Draw(GetTexture(name), &clip, nullptr, &position3, MixColors(color, ambientColor, 0.5f));

			break;
		}

		case RenderObjectType::Text:
		{
			auto textObject = reinterpret_cast<TextRenderObject*>(object);

			LPD3DXFONT pFont;
			textObject->GetFont(&pFont);

			std::wstring text;
			textObject->GetText(&text);

			D3DCOLOR color;
			textObject->GetColor(&color);

			RECT rect;
			textObject->GetRect(&rect);

			DWORD format;
			textObject->GetFormat(&format);

			D3DXMATRIX rotation;
			textObject->GetRotation(&rotation);

			pSprite->SetTransform(&rotation);
			pFont->DrawText(pSprite, text.c_str(), -1, &rect, format, MixColors(color, ambientColor, 0.5f));

			break;
		}

		case RenderObjectType::Custom:
		{
			auto customObject = reinterpret_cast<IRenderObject*>(object);
			customObject->Render(pSprite);
			break;
		}
		}

	}
	}

void Renderer::RenderWorld() const
{
	auto sprite = pDirectX->GetSprite();

	HRESULT result = sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
	assert(SUCCEEDED(result));

	D3DXMATRIX oldTransform;
	sprite->GetTransform(&oldTransform);

	RenderTiles(worldToScreenTransform);
	RenderObjectList(objects, worldToScreenTransform);

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	RenderObjectList(uiObjects, identity);

	sprite->End();
}

void Renderer::Render() const
{
	LPDIRECT3DDEVICE9 pDevice = pDirectX->GetDevice();

	HRESULT result = pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	if (SUCCEEDED(result = pDevice->BeginScene()))
	{
		float timeElapsed = MeasureTime([&]() { RenderWorld(); });

		static bool drawDebug = false;
		if (pDirectX->IsKeyPressed(DIK_F1))
			drawDebug = !drawDebug;

		if (drawDebug && debugDraw != nullptr)
		{
			timeElapsed = MeasureTime([&]() { physics->DrawDebugData(); });
		}

		result = pDevice->EndScene();
		assert(SUCCEEDED(result));
		pDevice->Present(nullptr, nullptr, nullptr, nullptr);
	}
	else
		DebugBreak();
}

bool Renderer::AddRenderObject(IRenderObject* pObject)
{
	objects.push_front(pObject);
	return true;
}

bool Renderer::AddUIObject(IRenderObject* pObject)
{
	uiObjects.push_front(pObject);
	return true;
}

bool Renderer::RemoveRenderObject(IRenderObject* pObject)
{
	auto iter = std::find(objects.begin(), objects.end(), pObject);
	if (iter != objects.end())
		objects.erase(iter);
	return true;
}

bool Renderer::RemoveUIObject(IRenderObject* pObject)
{
	auto iter = std::find(uiObjects.begin(), uiObjects.end(), pObject);
	if (iter != uiObjects.end())
		uiObjects.erase(iter);
	return true;
}

const D3DXVECTOR2& Renderer::GetCameraPosition() const
{
	return cameraPosition;
}

void Renderer::SetCameraPosition(const D3DXVECTOR2& position)
{
	cameraPosition = position * float(PixelsPerTile);
	cameraPosition.x = float(int(cameraPosition.x));
	cameraPosition.y = float(int(cameraPosition.y));
	cameraPosition /= float(PixelsPerTile);

	UpdateTranforms();
}

const RECT& Renderer::GetGameArea() const
{
	return gameArea;
}

void Renderer::SetGameArea(const RECT& gameArea)
{
	this->gameArea = gameArea;

	UpdateTranforms();
}

const GameWorld* Renderer::GetGameWorld() const
{
	return gameWorld;
}

void Renderer::SetGameWorld(GameWorld* gameWorld)
{
	this->gameWorld = gameWorld;
}

bool Renderer::WorldToScreen(const D3DXVECTOR3& world, D3DXVECTOR2* pScreen) const
{
	D3DXVECTOR4 screen;
	D3DXVec3Transform(&screen, &world, &worldToScreenTransform);

	pScreen->x = float(int(screen.x));
	pScreen->y = float(int(screen.y));

	if (screen.x < gameArea.left || screen.y < gameArea.top || screen.x >= gameArea.right || screen.y >= gameArea.bottom)
		return false;

	return true;
}

bool Renderer::ScreenToWorld(const D3DXVECTOR2& screen, D3DXVECTOR3* pWorld) const
{
	D3DXVECTOR4 world;
	D3DXVec2Transform(&world, &screen, &screenToWorldTransform);

	pWorld->x = world.x;
	pWorld->y = world.y;
	pWorld->z = 0.0f;

	return true;
}

float Renderer::PixelsToGameUnits(int pixels) const
{
	return float(pixels) / PixelsPerTile;
}

int Renderer::GameUnitsToPixels(float units) const
{
	return int(units * float(PixelsPerTile));
}

void Renderer::SetAmbientColor(D3DCOLOR ambientColor)
{
	this->ambientColor = ambientColor;
}

void Renderer::UpdateTranforms()
{
	D3DXMATRIX translate1;
	D3DXMATRIX scale;
	D3DXMATRIX translate2;
	D3DXMATRIX rotate;
	D3DXMatrixTranslation(&translate1, -cameraPosition.x, -cameraPosition.y, 0.0f);
	D3DXMatrixScaling(&scale, float(PixelsPerTile), float(PixelsPerTile), 1.0f);
	D3DXMatrixTranslation(&translate2, float(gameArea.left + (gameArea.right - gameArea.left) / 2), float(gameArea.top + (gameArea.bottom - gameArea.top) / 2), 0.0f);

	worldToScreenTransform = translate1 * scale * translate2;
	D3DXMatrixInverse(&screenToWorldTransform, nullptr, &worldToScreenTransform);
}

DirectXManager* Renderer::GetDirectX() const
{
	return pDirectX;
}

void Renderer::SetPhysicsDraw(b2World* physics, PhysicsDebugDraw* debugDraw)
{
	this->physics = physics;
	this->debugDraw = debugDraw;
}

void Renderer::AddFont(const std::wstring& name, LPD3DXFONT font)
{
	font->AddRef();
	fontMap[name] = font;
}

void Renderer::RemoveFont(const std::wstring& name)
{
	auto iter = fontMap.find(name);
	if (iter != fontMap.end())
	{
		fontMap.erase(iter);
		iter->second->Release();
	}
}

LPD3DXFONT Renderer::GetFont(const std::wstring& name)
{
	return fontMap[name];
}