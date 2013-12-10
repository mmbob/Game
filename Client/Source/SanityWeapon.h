#pragma once

#include "Weapon.h"

class SanityBullet : public Bullet
{
public:
	SanityBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime);
	~SanityBullet();

	virtual std::wstring GetTypeName() const;
};

class SanityWeapon : public Weapon
{
public:
	SanityWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual bool Fire(const D3DXVECTOR2& start, float angle);
};

class DoubleSanityWeapon : public Weapon
{
public:
	DoubleSanityWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual bool Fire(const D3DXVECTOR2& start, float angle);
};