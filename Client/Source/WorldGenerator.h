#pragma once

#include "GameWorld.h"

class WorldGenerator
{
protected:
	virtual bool GenerateChunk(GameWorld& world, int x, int y, WorldChunk* chunk, );
public:
	WorldGenerator();
	virtual ~WorldGenerator();
};