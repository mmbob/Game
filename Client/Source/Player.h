#pragma once

#include "Renderer.h"
#include "Entity.h"
#include "DirectX.h"

class Player : public RenderedEntity
{
	int sanity;
	float maxSpeed;
	D3DXVECTOR2 acceleration;
public:
	Player(Renderer* pRenderer);
	~Player();

	void Update(DirectXManager* pDirectX);

	int GetSanity() const;
};