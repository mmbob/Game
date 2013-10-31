#include "RenderObject.h"

#include "Renderer.h"

IRenderObject::~IRenderObject()
{ }

void IRenderObject::Render(LPD3DXSPRITE pSprite) const
{ }

RenderObject::RenderObject(Renderer* pParent)
	: pParent(pParent), position(0.0f, 0.0f, 0.0f)
{
	D3DXMatrixIdentity(&rotation);
}

bool RenderObject::GetPosition(D3DXVECTOR3* pPosition) const
{
	*pPosition = position;
	return true;
}

bool RenderObject::GetRotation(D3DXMATRIX* pRotation) const
{
	*pRotation = rotation;
	return true;
}

bool RenderObject::SetPosition(const D3DXVECTOR3& position)
{
	this->position = position;
	return true;
}

bool RenderObject::SetRotation(const D3DXMATRIX& rotation)
{
	this->rotation = rotation;
	return true;
}

void RenderObject::Render(LPD3DXSPRITE pSprite) const
{ }

TextureRenderObject::TextureRenderObject(Renderer* pParent)
	: RenderObject(pParent)
{ }

TextureRenderObject::~TextureRenderObject()
{ }

bool TextureRenderObject::GetTextureName(wstring* pName) const
{
	*pName = textureName;
	return true;
}

bool TextureRenderObject::GetTextureClip(RECT* pClip) const
{
	*pClip = textureClip;
	return true;
}

bool TextureRenderObject::SetTextureName(const wstring& name)
{
	textureName = name;
	return true;
}

bool TextureRenderObject::SetTextureClip(const RECT& clip)
{
	textureClip = clip;
	return true;
}

RenderObjectType::Value TextureRenderObject::GetType() const
{
	return RenderObjectType::Texture;
}

TextRenderObject::TextRenderObject(Renderer* pParent)
: RenderObject(pParent), pFont(nullptr), format(0), color(D3DCOLOR_XRGB(0, 0, 0))
{ }

TextRenderObject::~TextRenderObject()
{ }

bool TextRenderObject::GetFont(LPD3DXFONT* ppFont) const
{
	*ppFont = pFont;
	return true;
}

bool TextRenderObject::GetText(wstring* pText) const
{
	*pText = text;
	return true;
}

bool TextRenderObject::GetColor(D3DCOLOR* pColor) const
{
	*pColor = color;
	return true;
}

bool TextRenderObject::GetRect(RECT* pRect) const
{
	*pRect = rect;
	return true;
}

bool TextRenderObject::GetFormat(DWORD* pFormat) const
{
	*pFormat = format;
	return true;
}

bool TextRenderObject::SetFont(LPD3DXFONT pFont)
{
	this->pFont = pFont;
	return true;
}

bool TextRenderObject::SetText(const wstring& text)
{
	this->text = text;
	return true;
}

bool TextRenderObject::SetColor(D3DCOLOR color)
{
	this->color = color;
	return true;
}

bool TextRenderObject::SetRect(const RECT& rect)
{
	this->rect = rect;
	return true;
}

bool TextRenderObject::SetFormat(DWORD format)
{
	this->format = format;
	return true;
}

RenderObjectType::Value TextRenderObject::GetType() const
{
	return RenderObjectType::Text;
}