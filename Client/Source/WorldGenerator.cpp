#include "WorldGenerator.h"

#include <Box2D/Box2D.h>

#include "Game.h"
#include "GameState.h"
#include "Renderer.h"
#include "Engine.h"
#include "Player.h"
#include "SanityCrystal.h"
#include "Enemy.h"
#include "TileDefs.h"

IWorldGenerator::~IWorldGenerator()
{ }

WorldGenerator::WorldGenerator(InGameState* parent, Renderer* renderer, Engine* engine)
: parent(parent), renderer(renderer), engine(engine)
{ }

WorldGenerator::~WorldGenerator()
{
	std::vector<Point> locations;
	for (std::pair<const Point, std::unique_ptr<ChunkBodyInfo> >& pair : bodyMap)
	{
		locations.push_back(pair.first);
	}

	for (Point location : locations)
		UnloadChunk(nullptr, location.x, location.y);
}

bool WorldGenerator::GenerateChunk(const GameWorld* world, int x, int y, WorldChunk* pChunk)
{
	WorldChunk& chunk = *pChunk;

	chunk.SetInitialized();

	int baseTiles[] = { TileName::Ground1, TileName::Ground2, TileName::Ground3, TileName::Ground4, TileName::Ground5 };

//	int seed = rand() % 1024;

	D3DXVECTOR3 playerPosition;
	parent->GetPlayer()->GetPosition(&playerPosition);

	Point noSpawnZone(int(playerPosition.x) - x * WorldChunk::ChunkSize, int(playerPosition.y) - y * WorldChunk::ChunkSize);

	for (int tileX = 0; tileX < WorldChunk::ChunkSize; ++tileX)
	for (int tileY = 0; tileY < WorldChunk::ChunkSize; ++tileY)
	{
		int baseTile = baseTiles[rand() % 5];
		int featureTile = TileName::Blank;
		if (std::abs(noSpawnZone.x - tileX) >= 2 || std::abs(noSpawnZone.y - tileY) >= 2)	// Make sure a rock or enemy doesn't spawn too close to player
		{
			const int SpawnRate = 3;

			int grassTileCount = 0;
			if (tileX > 0 && chunk.GetTile(1, tileX - 1, tileY).ID == TileName::TallGrass1)
				grassTileCount += 25;
			if (tileY > 0 && chunk.GetTile(1, tileX, tileY - 1).ID == TileName::TallGrass1)
				grassTileCount += 25;

			int randValue = rand() % 100;
			if (randValue < 2 * SpawnRate)
				featureTile = TileName::Rock1;
			else if (randValue < 4 * SpawnRate)
				featureTile = TileName::Rock2;
			else if (randValue < 5 * SpawnRate)
				featureTile = TileName::Hole1;
			else if (randValue < 6 * SpawnRate + 2 + grassTileCount)
				featureTile = TileName::TallGrass1;

			if (featureTile == TileName::Blank)
			{
				if (rand() % 30 == 0)
					featureTile = TileName::SanityCrystal;
				else if (rand() % 100 == 0)
					featureTile = TileName::Enemy1;
				else if (rand() % 100 == 0)
					featureTile = TileName::Enemy2;
				else if (rand() % 100 == 0)
					featureTile = TileName::Enemy3;
				else if (rand() % 100 == 0)
					featureTile = TileName::Enemy4;
			}
		}

		chunk.SetTile(0, tileX, tileY, world->GetTileList()[baseTile]);
		chunk.SetTile(1, tileX, tileY, world->GetTileList()[featureTile]);
	}

	return true;
}

bool WorldGenerator::LoadChunk(const GameWorld* world, int chunkX, int chunkY)
{
	Point location(chunkX, chunkY);
	bodyMap[location].reset(new ChunkBodyInfo);
	const WorldChunk& chunk = world->GetChunk(chunkX, chunkY);

	for (int x = 0; x < WorldChunk::ChunkSize; ++x)
	for (int y = 0; y < WorldChunk::ChunkSize; ++y)
	{
		Point worldPosition(chunkX * WorldChunk::ChunkSize + x, chunkY * WorldChunk::ChunkSize + y);
		D3DXVECTOR3 position3(float(worldPosition.x), float(worldPosition.y), 0.5f);

		const Tile& tile = chunk.GetTile(1, x, y);

		if ((tile.Flags & TileFlags::BlockMovement) != 0 || (tile.Flags & TileFlags::BlockSight) != 0)
		{
			CreateTileEntity(chunkX, chunkY, worldPosition, tile);
		}
		else if (tile.ID == TileName::SanityCrystal)
		{
			SanityCrystal* crystal = new SanityCrystal(renderer, engine);
			crystal->SetPosition(position3);

			bodyMap[location]->Entities.push_back(crystal);

			engine->AddEntity(crystal, EntityType::Static);
		}
		else if (tile.ID >= TileName::Enemy1 && tile.ID <= TileName::Enemy4)
		{
			Enemy* enemy = nullptr;
			if (tile.ID == TileName::Enemy1)
				enemy = new CloudEnemy(renderer, engine);
			else if (tile.ID == TileName::Enemy2)
				enemy = new SpiderEnemy(renderer, engine);
			else if (tile.ID == TileName::Enemy3)
				enemy = new PuddleEnemy(renderer, engine);
			else if (tile.ID == TileName::Enemy4)
				enemy = new GhostEnemy(renderer, engine);

			enemy->SetPosition(position3);

			bodyMap[location]->Entities.push_back(enemy);

			engine->AddEntity(enemy, EntityType::Dynamic);
		}
	}

	return true;
}

bool WorldGenerator::UnloadChunk(const GameWorld*, int chunkX, int chunkY)
{
	Point location(chunkX, chunkY);

	std::unique_ptr<ChunkBodyInfo>& bodyInfo = bodyMap[location];
	if (engine->GetPhysics() != nullptr)
		for (b2Body* body : bodyInfo->Bodies)
			engine->GetPhysics()->DestroyBody(body);
	for (BodyUserData* userData : bodyInfo->UserDatas)
		delete userData;
	for (Entity* entity : bodyInfo->Entities)
		engine->RemoveEntity(entity);

	bodyMap.erase(location);

	return true;
}

void WorldGenerator::CreateTileEntity(int chunkX, int chunkY, const Point& worldPosition, const Tile& tile)
{
	Point chunkLocation(chunkX, chunkY);

	bool blockMovement = (tile.Flags & TileFlags::BlockMovement) > 0;
	bool blockSight = (tile.Flags & TileFlags::BlockSight) > 0;

	if (blockMovement || blockSight)
	{
		b2PolygonShape shape;
		if (tile.Flags & TileFlags::BlockRectangle)
		{
			b2Vec2 points[4];

			float tileLeft = renderer->PixelsToGameUnits(tile.BlockRect.left) - 0.5f;
			float tileTop = renderer->PixelsToGameUnits(tile.BlockRect.top) - 0.5f;
			float tileRight = renderer->PixelsToGameUnits(tile.BlockRect.right) - 0.5f;
			float tileBottom = renderer->PixelsToGameUnits(tile.BlockRect.bottom) - 0.5f;

			points[0].Set(tileLeft, tileTop);
			points[1].Set(tileRight, tileTop);
			points[2].Set(tileRight, tileBottom);
			points[3].Set(tileLeft, tileBottom);

			shape.Set(points, 4);
		}
		else
		{
			std::unique_ptr<b2Vec2[]> points(new b2Vec2[tile.BlockPoints.size()]);
			for (size_t i = 0; i < tile.BlockPoints.size(); ++i)
			{
				b2Vec2 point;
				point.x = renderer->PixelsToGameUnits(tile.BlockPoints[i].x) - 0.5f;
				point.y = renderer->PixelsToGameUnits(tile.BlockPoints[i].y) - 0.5f;
				points[i] = point;
			}

			shape.Set(points.get(), tile.BlockPoints.size());
		}

		b2BodyDef tileBodyDef;
		tileBodyDef.type = b2_staticBody;
		tileBodyDef.position = b2Vec2(float(worldPosition.x), float(worldPosition.y));
		tileBodyDef.fixedRotation = true;

		BodyUserData* userData = new BodyUserData;
		userData->Type = BodyUserData::DataType::Tile;
		userData->Data.Tile = &tile;
		bodyMap[chunkLocation]->UserDatas.push_back(userData);

		tileBodyDef.userData = userData;

		b2Body* tileBody = engine->GetPhysics()->CreateBody(&tileBodyDef);

		b2FixtureDef tileFixtureDef;
		tileFixtureDef.shape = &shape;
		tileFixtureDef.density = 0.0f;
		tileFixtureDef.friction = 0.0f;
		tileFixtureDef.restitution = 0.0f;
		tileFixtureDef.isSensor = !blockMovement && blockSight;

		tileBody->CreateFixture(&tileFixtureDef);

		bodyMap[chunkLocation]->Bodies.push_back(tileBody);
	}
}