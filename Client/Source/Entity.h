#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <memory>

#include "Renderer.h"
#include "RenderObject.h"

using namespace std;

class IEntity
{
public:
	virtual ~IEntity() = 0;

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const = 0;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const = 0;

	virtual bool SetPosition(const D3DXVECTOR3& position) = 0;
	virtual bool SetRotation(const D3DXMATRIX& rotation) = 0;
};

class Entity : public IEntity
{
protected:
	D3DXVECTOR3 position;
	D3DXMATRIX rotation;
	D3DXVECTOR2 velocity;
public:
	Entity();

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const;
	virtual bool GetVelocity(D3DXVECTOR2* pVelocity) const;

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(const D3DXMATRIX& rotation);
	virtual bool SetVelocity(const D3DXVECTOR2& velocity);
};

class RenderedEntity : public Entity
{
protected:
	RenderObject* pRenderObject;
public:
	RenderedEntity(Renderer* pRenderer);
	virtual ~RenderedEntity();

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(const D3DXMATRIX& rotation);
};