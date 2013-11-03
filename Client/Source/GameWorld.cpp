#include "GameWorld.h"

#include <cassert>

#include "TileFileParser.h"

#pragma comment(lib, "libjsond.lib")

void GameWorld::Init(std::unique_ptr<WorldGenerator> generator)
{
	this->generator = std::unique_ptr<WorldGenerator>(generator.release());

	TileFileParser fileParser(this);
	fileParser.LoadFile(L"Resources\\Data\\Tiles.json");
}

const Tile& GameWorld::GetTile(int layer, int x, int y) const
{
	const WorldChunk& chunk = GetChunk(x / WorldChunk::ChunkSize, y / WorldChunk::ChunkSize);
	
	return chunk.GetTile(layer, x % WorldChunk::ChunkSize, y % WorldChunk::ChunkSize);
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