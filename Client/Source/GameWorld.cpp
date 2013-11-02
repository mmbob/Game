#include "GameWorld.h"

#include <cassert>

#include "Util.h"

void GameWorld::Init(std::unique_ptr<WorldGenerator> generator)
{
	this->generator = std::unique_ptr<WorldGenerator>(generator.release());

	tileList[TileName::Ground1].Tileset = 0;
	tileList[TileName::Ground1].TextureClip = Rect(64, 0, 128, 64);
	tileList[TileName::Ground1].Flags = 0;

	tileList[TileName::Ground2].Tileset = 0;
	tileList[TileName::Ground2].TextureClip = Rect(128, 0, 192, 64);
	tileList[TileName::Ground2].Flags = 0;

	tileList[TileName::Ground3].Tileset = 0;
	tileList[TileName::Ground3].TextureClip = Rect(192, 0, 256, 64);
	tileList[TileName::Ground3].Flags = 0;

	tileList[TileName::Ground4].Tileset = 0;
	tileList[TileName::Ground4].TextureClip = Rect(256, 0, 256 + 64, 64);
	tileList[TileName::Ground4].Flags = 0;
}

const WorldChunk& GameWorld::GetChunk(int x, int y) const
{
	WorldChunk& chunk = chunks[y % 40][x % 40];
	if (chunk.IsInitialized())
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

WorldChunk::WorldChunk() : initialized(false), layerCount(0)
{ }

void WorldChunk::SetInitialized()
{
	initialized = true;
}

bool WorldChunk::IsInitialized() const
{
	return initialized;
}

const Tile& WorldChunk::GetTile(int layer, int x, int y) const
{
	return *tiles[layer][y][x];
}

void WorldChunk::SetTile(int layer, int x, int y, const Tile& tile)
{
	assert(layer < MaxLayerCount);
	tiles[layer][y][x] = &tile;

	if (layer >= layerCount)
		layerCount = layer + 1;
}

int WorldChunk::GetLayerCount() const
{
	return layerCount;
}