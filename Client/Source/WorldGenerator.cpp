#include "WorldGenerator.h"

#include "TileDefs.h"

int tileData0[WorldChunk::ChunkSize][WorldChunk::ChunkSize] =
{
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
	{ TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1, TileName::Ground1 },
};

WorldGenerator::WorldGenerator()
{ }

WorldGenerator::~WorldGenerator()
{ }

bool WorldGenerator::GenerateChunk(const GameWorld& world, int x, int y, WorldChunk* pChunk)
{
	WorldChunk& chunk = *pChunk;

	chunk.SetInitialized();

	int possibleTiles[] = { TileName::Ground1, TileName::Ground2, TileName::Ground3, TileName::Ground4, TileName::Ground5 };

	int seed = rand() % 1024;

	for (int tileY = 0; tileY < WorldChunk::ChunkSize; ++tileY)
	for (int tileX = 0; tileX < WorldChunk::ChunkSize; ++tileX)
		chunk.SetTile(0, tileX, tileY, world.GetTileList()[possibleTiles[rand() % 5]]);

	return true;
}