#pragma once

#include "Renderer.h"
#include "Entity.h"
#include "DirectX.h"

class Player : public RenderedEntity
{
	int sanity;
public:
	Player(Renderer* pRenderer);
	~Player();

	void Update(DirectXManager* pDirectX);
};