#include "Renderer.h"

#include <array>
#include <cassert>
#include <algorithm>

#include "GameWorld.h"

void Renderer::Init(DirectXManager* pDirectX, HWND window)
{
	this->pDirectX = pDirectX;

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

	LoadTextures();
}

void Renderer::UnInit()
{
	pSanityFont->Release();
	pDayTimeFont->Release();
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
	pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	pSprite->Draw(GetTexture(L"Background"), nullptr, nullptr, &D3DXVECTOR3(0.0f, 500.0f, 0.0f), 0xFFFFFFFF);
	pSprite->Draw(GetTexture(L"Sanity"), nullptr, nullptr, &D3DXVECTOR3(10.0f, 520.0f, 0.0f), 0xFFFFFFFF);

	static int alpha = 255;
	alpha -= 2;
	if (alpha <= 0)
		alpha = 255;

	RECT transformedRect = sanityUpdateRect;

	pSanityFont->DrawText(pSprite, sanityText.c_str(), -1, const_cast<RECT*>(&sanityRect), DT_SINGLELINE, 0xFFFFFFFF);
	pSanityFont->DrawText(pSprite, sanityUpdateText.c_str(), -1, &transformedRect, DT_SINGLELINE, D3DCOLOR_ARGB(alpha, 255, 255, 255));
	pDayTimeFont->DrawText(pSprite, dayText.c_str(), -1, const_cast<RECT*>(&dayRect), DT_SINGLELINE, 0xFFFFFFFF);
	pDayTimeFont->DrawText(pSprite, timeText.c_str(), -1, const_cast<RECT*>(&timeRect), DT_SINGLELINE, 0xFFFFFFFF);

	pSprite->End();
}

void Renderer::RenderTiles() const
{
	auto pSprite = pDirectX->GetSprite();

	LPDIRECT3DTEXTURE9 tileSets[] = 
	{
		GetTexture(L"Tiles1"),
	};

	GameWorld* pWorld;
	array<WorldChunk, 4> chunksToRender =
	{
		pWorld->GetChunk(int(cameraPosition.x), int(cameraPosition.y)),
		pWorld->GetChunk(int(cameraPosition.x + 1), int(cameraPosition.y)),
		pWorld->GetChunk(int(cameraPosition.x), int(cameraPosition.y + 1)),
		pWorld->GetChunk(int(cameraPosition.x + 1), int(cameraPosition.y + 1)),
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

				pSprite->Draw(tileSets[tile.Tileset], &tile.TextureClip, nullptr, &D3DXVECTOR3(x * 64 - cameraOffset.x, y * 64 - cameraOffset.y, 1.0f), 0xFFFFFFFF);
			}
		}
	}
}

void Renderer::RenderWorld() const
{
	auto pSprite = pDirectX->GetSprite();

	pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXMATRIX oldTransform;
	pSprite->GetTransform(&oldTransform);

	RenderTiles();

	for (IRenderObject* object : objects)
	{
		RECT clip;
		object->GetTextureClip(&clip);

		wstring name;
		object->GetTextureName(&name);
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

			object->SetTextureClip(clip);
		}

		D3DXMATRIX rotation;
		object->GetRotation(&rotation);

		D3DXVECTOR3 position;
		object->GetPosition(&position);

		pSprite->SetTransform(&rotation);
		pSprite->Draw(GetTexture(name), &clip, nullptr, &position, 0xFFFFFFFF);
	}

	pSprite->SetTransform(&oldTransform);

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
	objects.push_back(pObject);
	return true;
}