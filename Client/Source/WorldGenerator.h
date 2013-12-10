#pragma once

#include <unordered_map>

#include "GameWorld.h"
#include "Util.h"

class Renderer;
class Engine;
class GameWorld;
class WorldChunk;
class InGameState;
class b2Body;
class Entity;
struct BodyUserData;
struct Tile;

class IWorldGenerator
{
public:
	virtual ~IWorldGenerator() = 0;

	virtual bool GenerateChunk(const GameWorld& world, int x, int y, WorldChunk* chunk) = 0;
	virtual bool LoadChunk(const GameWorld& world, int x, int y, const WorldChunk& chunk) = 0;
	virtual bool UnloadChunk(const GameWorld& world, int x, int y, const WorldChunk& chunk) = 0;
};

class WorldGenerator : public IWorldGenerator
{
	struct ChunkBodyInfo
	{
		std::vector<b2Body*> Bodies;
		std::vector<Entity*> Entities;
		std::vector<BodyUserData*> UserDatas;
	};

	std::unordered_map<Point, std::unique_ptr<ChunkBodyInfo>, GameLocationHash> bodyMap;

	InGameState* parent;
	Engine* engine;
	Renderer* renderer;

	void CreateTileEntity(int chunkX, int chunkY, const Point& worldPosition, const Tile& tile);
public:
	WorldGenerator(InGameState* parent, Renderer* renderer, Engine* engine);
	virtual ~WorldGenerator();

	virtual bool GenerateChunk(const GameWorld& world, int x, int y, WorldChunk* chunk);
	virtual bool LoadChunk(const GameWorld& world, int x, int y, const WorldChunk& chunk);
	virtual bool UnloadChunk(const GameWorld& world, int x, int y, const WorldChunk& chunk);
};