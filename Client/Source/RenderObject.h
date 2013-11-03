#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "Util.h"

class Renderer;

using namespace std;

struct RenderObjectType
{
	enum Value
	{
		Texture,
		Text,
		Custom,
	};
};

class IRenderObject
{
public:
	virtual ~IRenderObject() = 0;

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const = 0;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const = 0;

	virtual bool SetPosition(const D3DXVECTOR3& position) = 0;
	virtual bool SetRotation(const D3DXMATRIX& rotation) = 0;

	virtual void Render(LPD3DXSPRITE pSprite) const = 0;

	virtual RenderObjectType::Value GetType() const = 0;

	friend class Renderer;
};

class RenderObject : public IRenderObject
{
protected:
	Renderer* pParent;
	D3DXVECTOR3 position;
	D3DXMATRIX rotation;
public:
	RenderObject(Renderer* pParent);

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const;

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(const D3DXMATRIX& rotation);

	virtual void Render(LPD3DXSPRITE pSprite) const;

	virtual RenderObjectType::Value GetType() const = 0;
};

class TextureRenderObject : public RenderObject
{
protected:
	wstring textureName;
	Rect textureClip;
public:
	TextureRenderObject(Renderer* pParent);
	virtual ~TextureRenderObject();

	virtual bool GetTextureName(wstring* pName) const;
	virtual bool GetTextureClip(RECT* pClip) const;

	virtual bool SetTextureName(const wstring& name);
	virtual bool SetTextureClip(const RECT& clip);

	virtual RenderObjectType::Value GetType() const;
};

class TextRenderObject : public RenderObject
{
protected:
	LPD3DXFONT pFont;
	wstring text;
	DWORD format;
	Rect rect;
	D3DCOLOR color;
public:
	TextRenderObject(Renderer* pParent);
	virtual ~TextRenderObject();

	bool GetFont(LPD3DXFONT* ppFont) const;
	bool GetText(wstring* pText) const;
	bool GetColor(D3DCOLOR* pColor) const;
	bool GetRect(RECT* pRect) const;
	bool GetFormat(DWORD* pFormat) const;

	bool SetFont(LPD3DXFONT pFont);
	bool SetText(const wstring& text);
	bool SetColor(D3DCOLOR color);
	bool SetRect(const RECT& rect);
	bool SetFormat(DWORD format);

	virtual RenderObjectType::Value GetType() const;
};