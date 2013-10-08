#include "RenderObject.h"

#include "Renderer.h"

IRenderObject::~IRenderObject()
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

/*TextureRenderObject::TextureRenderObject(Renderer* pParent)
	: RenderObject(pParent)
{ }*/

bool RenderObject::GetTextureName(wstring* pName) const
{
	*pName = textureName;
	return true;
}

bool RenderObject::GetTextureClip(RECT* pClip) const
{
	*pClip = textureClip;
	return true;
}

bool RenderObject::SetTextureName(const wstring& name)
{
	textureName = name;
	return true;
}

bool RenderObject::SetTextureClip(const RECT& clip)
{
	textureClip = clip;
	return true;
}

/*TextRenderObject::TextRenderObject(Renderer* pParent)
	: RenderObject(pParent)
{ }

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

bool TextRenderObject::SetText(const wstring& text)
{
	this->text = text;
	return true;
}

bool TextRenderObject::SetColor(D3DCOLOR color)
{
	this->color = color;
	return true;
}*/