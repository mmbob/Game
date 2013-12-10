#pragma once

#include <set>
#include <string>

#include "Entity.h"

class IWeapon;
class Weapon;

class IBullet : public RenderedEntity
{
public:
	IBullet(Renderer* pRenderer, Engine* pEngine, IRenderObject* pRenderObject);
	virtual ~IBullet() = 0;

	virtual void Update(float timeElapsed) = 0;

	virtual IWeapon* GetWeapon() const = 0;

	virtual void AddHitEntity(IEntity* entity) = 0;
	virtual bool HasHitEntity(IEntity* entity) const = 0;

	virtual int GetDamage() const = 0;
	virtual bool IsDestroyedOnHit() const = 0;
	virtual bool IsLifeOver() const = 0;
};

class Bullet : public IBullet
{
protected:
	IWeapon* parent;
	Renderer* renderer;
	Engine* engine;
	float endOfLifeTime;
	std::set<IEntity*> hitEntities;
public:
	Bullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, IRenderObject* pRenderObject, float lifeTime);

	virtual void Update(float timeElapsed);

	virtual IWeapon* GetWeapon() const;

	virtual void AddHitEntity(IEntity* entity);
	virtual bool HasHitEntity(IEntity* entity) const;

	virtual int GetDamage() const;
	virtual bool IsDestroyedOnHit() const;
	virtual bool IsLifeOver() const;

	virtual std::wstring GetTypeName() const;
};

class IWeapon
{
public:
	virtual ~IWeapon() = 0;

	virtual bool Fire(const D3DXVECTOR2& start, float angle) = 0;

	virtual void Update(float timeElapsed) = 0;

	virtual Entity* GetParent() const = 0;

	virtual bool CanFire() const = 0;
};

class Weapon : public IWeapon
{
protected:
	Entity* parent;
	Renderer* renderer;
	Engine* engine;

	float lastFireTime;
	float fireCooldown;
	std::list<Bullet*> bullets;
	std::vector<std::pair<Bullet*, BodyUserData*> > bulletCollisions;
public:
	Weapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual void SetFireCooldown(float fireCooldown);
	virtual float GetFireCooldown() const;

	virtual void Update(float timeElapsed);

	virtual const std::vector<std::pair<Bullet*, BodyUserData*> >& GetBulletCollisions() const;
	virtual void RemoveBullet(Bullet* bullet);

	virtual Entity* GetParent() const;

	virtual bool CanFire() const;
};