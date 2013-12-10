#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <memory>
#include <Box2D/Box2D.h>

#include "Engine.h"
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
	virtual bool GetRotation(float* pAngle) const = 0;

	virtual bool SetPosition(const D3DXVECTOR3& position) = 0;
	virtual bool SetRotation(float angle) = 0;

	virtual std::wstring GetTypeName() const = 0;
};

struct BodyUserData;

class Entity : public IEntity
{
protected:
	Engine* parent;

	float zIndex;
	b2Body* body;
	BodyUserData userData;
	int flags;

	int health;
	int lastDamageTime;
	int recentHealthChange;
public:
	Entity(Engine* engine);

	virtual bool GetPosition(D3DXVECTOR3* pPosition) const;
	virtual bool GetRotation(float* pAngle) const;
	virtual bool GetVelocity(D3DXVECTOR2* pVelocity) const;
	virtual bool GetFlags(int* pFlags) const;
	virtual bool GetBody(b2Body** pBody) const;

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(float angle);
	virtual bool SetVelocity(const D3DXVECTOR2& velocity);
	virtual bool SetFlags(int flags);
	virtual bool SetBody(b2Body* body);

	virtual void SetAlive(bool alive);
	virtual bool IsAlive() const;

	virtual int GetHealth() const;
	virtual int Damage(int amount);

	virtual void Update(float timeElapsed);

	virtual std::wstring GetTypeName() const;
};

class RenderedEntity : public Entity
{
protected:
	std::unique_ptr<IRenderObject> renderObject;
	Renderer* renderer;

	void UpdateRenderObject(bool updatePosition, bool updateRotation);
public:
	RenderedEntity(Renderer* pRenderer, Engine* pEngine, IRenderObject* renderObject);
	virtual ~RenderedEntity();

	virtual bool SetPosition(const D3DXVECTOR3& position);
	virtual bool SetRotation(float angle);

	virtual void SetAlive(bool alive);

	virtual std::wstring GetTypeName() const;
};