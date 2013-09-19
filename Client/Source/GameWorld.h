#pragma once

#include <string>

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
	std::string TextureName;
	float TextureU;
	float TextureV;
	TileFlags::Value Flags;
};

struct WorldChunk
{
	static const int ChunkSize = 16;

	Tile tiles[ChunkSize][ChunkSize];
};

class GameWorld
{
	static const int WorldSize = 40;		// The world is a WorldSize x WorldSize square

	WorldChunk chunks[WorldSize][WorldSize];
public:
};