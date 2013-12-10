#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <deque>
#include <array>

#include "TileDefs.h"
#include "Util.h"

struct TileFlags
{
	enum Value
	{
		BlockSight = 1,
		BlockMovement = 2,
		BlockRectangle = 4,
		BlockPolygon = 8,
	};
};

struct Tile
{
	int ID;
	int Tileset;
	Rect TextureClip;
	int Flags;

	Rect BlockRect;							// BlockRectangle
	std::vector<POINT> BlockPoints;			// BlockPolygon
};

class IWorldGenerator;
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

class IWorldGenerator;

typedef void (*LoadChunkCallback)(int x, int y, WorldChunk& chunk, void* userData);

class GameWorld
{
	static const int WorldSize = 40;		// The world is a WorldSize x WorldSize square of chunks

	std::unique_ptr<IWorldGenerator> generator;

	Tile tileList[TileName::Count];
	mutable WorldChunk chunks[WorldSize][WorldSize];
	bool isChunkLoaded[WorldSize][WorldSize];
public:
	void Init(IWorldGenerator* generator);

	const Tile& GetTile(int layer, int x, int y) const;

	const WorldChunk& GetChunk(int x, int y) const;
	void SetChunk(int x, int y, const WorldChunk& chunk);

	bool IsChunkInitialized(int x, int y) const;

	void LoadChunk(int x, int y);
	void UnloadChunk(int x, int y);

	bool IsChunkLoaded(int x, int y) const;

	const Tile* GetTileList() const;
};

struct GameLocationHash
{
	size_t operator ()(const Point& value) const;
};