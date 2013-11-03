#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

class Renderer;

using namespace std;

class IRenderObject
{
public:
	virtual ~IRenderObject() = 0;

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const = 0;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const = 0;

	virtual bool SetPosition(const D3DXVECTOR3& position) = 0;
	virtual bool SetRotation(const D3DXMATRIX& rotation) = 0;

	virtual bool GetTextureName(wstring* pName) const = 0;
	virtual bool GetTextureClip(RECT* pClip) const = 0;

	virtual bool SetTextureName(const wstring& name) = 0;
	virtual bool SetTextureClip(const RECT& clip) = 0;

	friend class Renderer;
};

class RenderObject : public IRenderObject
{
protected:
	Renderer* pParent;
	D3DXVECTOR3 position;
	D3DXMATRIX rotation;

	wstring textureName;
	RECT textureClip;
public:
	RenderObject(Renderer* pParent);

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const;

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(const D3DXMATRIX& rotation);

	virtual bool GetTextureName(wstring* pName) const;
	virtual bool GetTextureClip(RECT* pClip) const;

	virtual bool SetTextureName(const wstring& name);
	virtual bool SetTextureClip(const RECT& clip);
};

/*class TextureRenderObject : public RenderObject
{
protected:
public:
	TextureRenderObject(Renderer* pParent);
	virtual ~TextureRenderObject();
};

class TextRenderObject : public RenderObject
{
protected:
	wstring text;
	D3DCOLOR color;
public:
	TextRenderObject(Renderer* pParent);
	virtual ~TextRenderObject();

	bool GetText(wstring* pText) const;
	bool GetColor(D3DCOLOR* pColor) const;

	bool SetText(const wstring& text);
	bool SetColor(D3DCOLOR color);
};*/