#pragma once

#include "Entity.h"
#include "DirectX.h"

class Player : public Entity
{
	int sanity;
public:
	Player();

	void Update(DirectXManager* pDirectX);
};