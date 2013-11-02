#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <array>

#include "WorldGenerator.h"
#include "TileDefs.h"
#include "Util.h"

struct TileFlags
{
	enum Value
	{
		BlockSight,
		BlockMovement,
		BlockRectangle,
		BlockPolygon,
	};
};

struct Tile
{
	int Tileset;
	Rect TextureClip;
	int Flags;

	Rect BlockRect;							// BlockRectangle
	std::vector<POINT> BlockPoints;			// BlockPolygon
};

class GameWorld;

class WorldChunk
{
public:
	static const int MaxLayerCount = 4;
	static const int ChunkSize = 16;
private:
	bool initialized;
	const Tile* tiles[MaxLayerCount][ChunkSize][ChunkSize];
	int layerCount;
public:
	WorldChunk();

	void SetInitialized();
	bool IsInitialized() const;

	const Tile& GetTile(int layer, int x, int y) const;
	void SetTile(int layer, int x, int y, const Tile& tile);

	int GetLayerCount() const;
};

class WorldGenerator;

class GameWorld
{
	static const int WorldSize = 40;		// The world is a WorldSize x WorldSize square of chunks

	std::unique_ptr<WorldGenerator> generator;

	Tile tileList[TileName::Count];
	mutable WorldChunk chunks[WorldSize][WorldSize];
public:
	void Init(std::unique_ptr<WorldGenerator> generator);

	const Tile& GetTile(int layer, int x, int y) const;

	const WorldChunk& GetChunk(int x, int y) const;
	void SetChunk(int x, int y, const WorldChunk& chunk);

	const Tile* GetTileList() const;
};