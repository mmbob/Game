#include "GameWorld.h"

WorldChunk::WorldChunk() : Initialized(false)
{ }

void GameWorld::Init(WorldGenerator* pGenerator)
{
	generator = std::unique_ptr<WorldGenerator>(pGenerator);
}

const WorldChunk& GameWorld::GetChunk(int x, int y) const
{
	WorldChunk& chunk = chunks[y][x];
	if (chunk.Initialized)
		return chunk;

	generator->GenerateChunk(*this, x, y, &chunk);

	return chunk;
}

void GameWorld::SetChunk(int x, int y, const WorldChunk& chunk)
{
	chunks[y][x] = chunk;
}

const Tile* GameWorld::GetTileList() const
{
	return tileList;
}