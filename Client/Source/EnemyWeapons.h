#pragma once

#include "Weapon.h"
#include "SanityWeapon.h"

class SpiderMeleeBullet : public Bullet
{
public:
	SpiderMeleeBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine);

	virtual int GetDamage() const;
};

class SpiderMeleeWeapon : public Weapon
{
public:
	SpiderMeleeWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown);

	virtual bool Fire(const D3DXVECTOR2& start, float angle);
};