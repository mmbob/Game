#include "Renderer.h"

#include <array>
#include <cassert>
#include <algorithm>

#include "GameWorld.h"

void Renderer::Init(DirectXManager* pDirectX, HWND window)
{
	this->pDirectX = pDirectX;
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

void Renderer::RenderUI() const
{
	LPD3DXSPRITE pSprite = pDirectX->GetSprite();
}

void Renderer::RenderTiles() const
{
	auto pSprite = pDirectX->GetSprite();

	LPDIRECT3DTEXTURE9 tileSets[] = 
	{
		GetTexture(L"Tiles1"),
	};

	GameWorld* pWorld = new GameWorld;
	WorldGenerator* pGenerator = new WorldGenerator;
	pWorld->Init(pGenerator);
	array<WorldChunk, 4> chunksToRender =
	{
		pWorld->GetChunk(int(cameraPosition.x / WorldChunk::ChunkSize), int(cameraPosition.y / WorldChunk::ChunkSize)),
		pWorld->GetChunk(int(cameraPosition.x / WorldChunk::ChunkSize + 1), int(cameraPosition.y / WorldChunk::ChunkSize)),
		pWorld->GetChunk(int(cameraPosition.x / WorldChunk::ChunkSize), int(cameraPosition.y / WorldChunk::ChunkSize + 1)),
		pWorld->GetChunk(int(cameraPosition.x / WorldChunk::ChunkSize + 1), int(cameraPosition.y / WorldChunk::ChunkSize + 1)),
	};

	D3DXVECTOR2 cameraOffset = cameraPosition - D3DXVECTOR2(int(cameraPosition.x), int(cameraPosition.y));
	cameraOffset *= (64 * WorldChunk::ChunkSize);

	for (const WorldChunk chunk : chunksToRender)
	{
		for (int x = 0; x < chunk.ChunkSize; ++x)
		{
			for (int y = 0; y < chunk.ChunkSize; ++y)
			{
				auto tile = chunk.GetTile(x, y);

				pSprite->Draw(tileSets[tile.Tileset], &tile.TextureClip, nullptr, &D3DXVECTOR3(x * 64, y * 64, 1.0f), ambientColor);
			}
		}
	}

	delete pWorld;
//	delete pGenerator;
}

void Renderer::RenderObjectList(const list<IRenderObject*>& list) const
{
	auto pSprite = pDirectX->GetSprite();

	for (IRenderObject* object : list)
	{
		switch (object->GetType())
		{
		case RenderObjectType::Texture:
		{

										  auto texObject = reinterpret_cast<TextureRenderObject*>(object);
										  RECT clip;
										  texObject->GetTextureClip(&clip);

										  wstring name;
										  texObject->GetTextureName(&name);
										  if (clip.right == -1 || clip.bottom == -1)
										  {
											  LPDIRECT3DTEXTURE9 pTexture = GetTexture(name);
											  if (pTexture != nullptr)
											  {
												  D3DSURFACE_DESC desc;
												  pTexture->GetLevelDesc(0, &desc);

												  clip.right = desc.Width;
												  clip.bottom = desc.Height;
											  }
											  else
											  {
												  clip.right = 1;
												  clip.bottom = 1;
											  }

											  texObject->SetTextureClip(clip);
										  }

										  D3DXMATRIX rotation;
										  texObject->GetRotation(&rotation);

										  D3DXVECTOR3 position;
										  texObject->GetPosition(&position);

										  //			pSprite->SetTransform(&rotation);
										  pSprite->Draw(GetTexture(name), &clip, nullptr, &position, ambientColor);

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

									   //			pSprite->SetTransform(&rotation);
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
	auto pSprite = pDirectX->GetSprite();

	HRESULT result = pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);

	D3DXMATRIX oldTransform;
	pSprite->GetTransform(&oldTransform);

	D3DVIEWPORT9 viewport;
	pDirectX->GetDevice()->GetViewport(&viewport);

	D3DXMATRIX transform;
	D3DXMatrixIdentity(&transform);
	D3DXMatrixTranslation(&transform, gameArea.left + (gameArea.right - gameArea.left) / 2 - cameraPosition.x, gameArea.top + (gameArea.bottom - gameArea.top) / 2 - cameraPosition.y, 0.0f);

	pSprite->SetTransform(&transform);

	D3DXMATRIX view;
	D3DXMATRIX world;
	pDirectX->GetDevice()->GetTransform(D3DTS_VIEW, &view);
	pDirectX->GetDevice()->GetTransform(D3DTS_WORLD, &world);
	pSprite->SetWorldViewLH(&world, &view);

	RenderTiles();

	RenderObjectList(objects);

	pSprite->SetTransform(&oldTransform);

	RenderObjectList(uiObjects);

	pSprite->End();
}

void Renderer::Render() const
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

const D3DXVECTOR2& Renderer::GetCameraPosition() const
{
	return cameraPosition;
}

void Renderer::SetCameraPosition(const D3DXVECTOR2& position)
{
	cameraPosition = position;
}

const RECT& Renderer::GetGameArea() const
{
	return gameArea;
}

void Renderer::SetGameArea(const RECT& gameArea)
{
	this->gameArea = gameArea;
}

bool Renderer::WorldToScreen(const D3DXVECTOR3& world, D3DXVECTOR2* pScreen) const
{
	D3DXVECTOR2 screen(world.x, world.y);
	screen *= WorldChunk::ChunkSize;

	pScreen->x = float(int(screen.x));
	pScreen->y = float(int(screen.y));

	return true;
}

bool Renderer::ScreenToWorld(const D3DXVECTOR2& screen, D3DXVECTOR3* pWorld) const
{
	D3DXVECTOR3 world(screen.x, screen.y, 0.0f);
	world /= WorldChunk::ChunkSize;

	*pWorld = world;

	return true;
}

void Renderer::SetAmbientColor(D3DCOLOR ambientColor)
{
	this->ambientColor = ambientColor;
}