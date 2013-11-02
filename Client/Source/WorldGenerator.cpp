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

	int baseTiles[] = { TileName::Ground1, TileName::Ground2, TileName::Ground3, TileName::Ground4, TileName::Ground5 };
	int featureTiles[] = { TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Blank, TileName::Rock1, TileName::Rock2 };

	int seed = rand() % 1024;

	for (int tileX = 0; tileX < WorldChunk::ChunkSize; ++tileX)
	for (int tileY = 0; tileY < WorldChunk::ChunkSize; ++tileY)
		chunk.SetTile(0, tileX, tileY, world.GetTileList()[baseTiles[rand() % 5]]);
	for (int tileX = 0; tileX < WorldChunk::ChunkSize; ++tileX)
	for (int tileY = 0; tileY < WorldChunk::ChunkSize; ++tileY)
		chunk.SetTile(1, tileX, tileY, world.GetTileList()[featureTiles[rand() % 12]]);

	return true;
}