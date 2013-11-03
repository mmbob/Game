#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <memory>
#include <Box2D/Box2D.h>

#include "Renderer.h"
#include "RenderObject.h"

using namespace std;

class Engine;

struct EntityFlags
{
	enum Value
	{
		Visible,
		Collides,
	};
};

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
	Engine* parent;

	D3DXVECTOR3 position;
	D3DXMATRIX rotation;
	D3DXVECTOR2 velocity;
	int flags;
	b2Body* body;
public:
	Entity(Engine* pEngine);

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const;
	virtual bool GetRotation(D3DXMATRIX* pRotation) const;
	virtual bool GetVelocity(D3DXVECTOR2* pVelocity) const;
	virtual bool GetFlags(int* pFlags) const;
	virtual bool GetBody(b2Body** pBody) const;

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(const D3DXMATRIX& rotation);
	virtual bool SetVelocity(const D3DXVECTOR2& velocity);
	virtual bool SetFlags(int flags);
	virtual bool SetBody(b2Body* body);
};

class RenderedEntity : public Entity
{
protected:
	std::unique_ptr<RenderObject> renderObject;
public:
	RenderedEntity(Renderer* pRenderer, Engine* pEngine);
	virtual ~RenderedEntity();

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(const D3DXMATRIX& rotation);
};