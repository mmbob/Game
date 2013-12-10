#pragma once

#include "Weapon.h"
#include "SanityWeapon.h"

class LightningBullet : public Bullet
{
public:
	LightningBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime);

	virtual int GetDamage() const;
	virtual bool IsDestroyedOnHit() const;
};

class LightningBoltWeapon : public Weapon
{
	float boltDistance;
public:
	LightningBoltWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual bool Fire(const D3DXVECTOR2& start, float angle);

	void SetBoltDistance(float distance);
};

class FlameBoltBullet : public Bullet
{
public:
	FlameBoltBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime);

	virtual int GetDamage() const;
	virtual bool IsDestroyedOnHit() const;
};

class FlameBoltWeapon : public Weapon
{
public:
	FlameBoltWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual bool Fire(const D3DXVECTOR2& start, float angle);
};

class ShockwaveBullet : public Bullet
{
public:
	enum Direction
	{
		UpRight = 0,
		UpLeft = 1,
		DownLeft = 2,
		DownRight = 3,
	};

	ShockwaveBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime, Direction direction);

	virtual int GetDamage() const;
	virtual bool IsDestroyedOnHit() const;
};

class ShockwaveWeapon : public Weapon
{
public:
	ShockwaveWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual bool Fire(const D3DXVECTOR2& start, float angle);
};