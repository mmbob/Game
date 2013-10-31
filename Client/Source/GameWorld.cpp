#include "GameWorld.h"

#include "Util.h"

WorldChunk::WorldChunk() : Initialized(false)
{ }

void GameWorld::Init(WorldGenerator* pGenerator)
{
	generator = std::unique_ptr<WorldGenerator>(pGenerator);

	tileList[TileName::Ground1].Tileset = 0;
	tileList[TileName::Ground1].TextureClip = Rect(0, 0, 64, 64);
	tileList[TileName::Ground1].Flags = TileFlags::Passable;
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

const Tile& WorldChunk::GetTile(int x, int y) const
{
	return *Tiles[y][x];
}