#pragma once

#include "Renderer.h"
#include "Entity.h"
#include "DirectX.h"
#include "PlayerWeapons.h"

class Player : public RenderedEntity
{
	struct WeaponInfo
	{
		int SanityCost;
		Weapon* Weapon;
	};

	Weapon* currentWeapon;
	int currentWeaponIndex;
	std::vector<WeaponInfo> weapons;

	int highestSanity;

	void HandleCollisions();
public:
	Player(Renderer* pRenderer, Engine* pEngine);
	~Player();

	virtual void Update(float timeElapsed);

	int GetSanity() const;
	int GetHighestSanity() const;

	void ChangeWeapon(int index);
	int GetCurrentWeaponCost() const;

	virtual std::wstring GetTypeName() const;
};