#pragma once

#include "Renderer.h"
#include "Entity.h"

class Player : RenderedEntity
{
public:
	Player(Renderer* pRenderer);
	~Player();
};