#pragma once

#include <Windows.h>
#include <string>
#include <memory>

#include "WorldGenerator.h"
#include "TileDefs.h"

struct TileFlags
{
	enum Value
	{
		BlocksSight,
		Passable,
	};
};

struct Tile
{
	int Tileset;
	RECT TextureClip;
	TileFlags::Value Flags;
};

struct WorldChunk
{
	static const int ChunkSize = 16;

	bool Initialized;
	const Tile* Tiles[ChunkSize][ChunkSize];

	WorldChunk();

	const Tile& GetTile(int x, int y) const;
	void SetTile(int x, int y, const Tile& tile);
};

class GameWorld
{
	static const int WorldSize = 40;		// The world is a WorldSize x WorldSize square

	std::unique_ptr<WorldGenerator> generator;

	Tile tileList[TileName::Count];
	mutable WorldChunk chunks[WorldSize][WorldSize];
public:
	void Init(WorldGenerator* pGenerator);

	const WorldChunk& GetChunk(int x, int y) const;
	void SetChunk(int x, int y, const WorldChunk& chunk);

	const Tile* GetTileList() const;
};