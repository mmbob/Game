#pragma once

#include "GameWorld.h"

class GameWorld;
struct WorldChunk;

class WorldGenerator
{
protected:
public:
	WorldGenerator();
	virtual ~WorldGenerator();

	virtual bool GenerateChunk(const GameWorld& world, int x, int y, WorldChunk* chunk);
};