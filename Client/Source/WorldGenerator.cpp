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

bool WorldGenerator::GenerateChunk(const GameWorld& world, int x, int y, WorldChunk* pChunk)
{
	WorldChunk& chunk = *pChunk;

	int seed = rand() % 1024;

	for (int tileY = 0; tileY < WorldChunk::ChunkSize; ++tileY)
	for (int tileX = 0; tileX < WorldChunk::ChunkSize; ++tileX)
		chunk.Tiles[tileY][tileX] = &world.GetTileList()[tileData0[tileY][tileX]];

	return true;
}