#include "Engine.h"

#include <set>
#include <map>

#include "Entity.h"
#include "GameWorld.h"

Engine::Engine()
: physics(new b2World(b2Vec2_zero))
{ }

void Engine::Init(GameWorld* world)
{
	this->world = world;

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.position.SetZero();
	
	worldBody = physics->CreateBody(&bodyDef);
}

void Engine::UnInit()
{
	for (Entity* entity : entitiesToRemove)
	{
		auto iter = std::find(dynamicEntities.begin(), dynamicEntities.end(), entity);
		if (iter != dynamicEntities.end())
			dynamicEntities.erase(iter);
		delete entity;
	}

	physics->DestroyBody(worldBody);

	physics.reset();
}

void Engine::Update(float timeElapsed)
{
	for (Entity* entity : entitiesToRemove)
	{
		b2Body* body;
		entity->GetBody(&body);
		physics->DestroyBody(body);

		auto iter = std::find(dynamicEntities.begin(), dynamicEntities.end(), entity);
		if (iter != dynamicEntities.end())
			dynamicEntities.erase(iter);
		delete entity;
	}

	entitiesToRemove.clear();

	const float timeStep = timeElapsed;// 1.0f / 60.0f;		// 60 steps per second
	const int velocityIterations = 8;
	const int positionIterations = 4;

	float physicsTime = MeasureTime([&]() { physics->Step(timeStep, velocityIterations, positionIterations); });
	physics->ClearForces();

	D3DXVECTOR3 playerPosition;
	player->GetPosition(&playerPosition);

	Point playerChunk(int(playerPosition.x / WorldChunk::ChunkSize), int(playerPosition.y / WorldChunk::ChunkSize));

	for (int x = -4; x <= 4; ++x)
	for (int y = -4; y <= 4; ++y)
	{
		Point location(playerChunk.x + x, playerChunk.y + y);
		if (std::abs(x) == 4 || std::abs(y) == 4)
		{
			if (world->IsChunkLoaded(location.x, location.y))
			{
				world->UnloadChunk(location.x, location.y);
			}
		}
		else if (world->IsChunkInitialized(location.x, location.y) && !world->IsChunkLoaded(location.x, location.y))
		{
			world->LoadChunk(location.x, location.y);
		}
	}

	for (Entity* entity : dynamicEntities)
		entity->Update(timeElapsed);
}

bool Engine::AddEntity(Entity* entity, EntityType::Value type)
{
	b2BodyDef bodyDef;
	switch (type)
	{
	case EntityType::Static:
		break;

	case EntityType::Dynamic:
		dynamicEntities.push_front(entity);
		break;

	default:
		return false;
	}

/*	b2PolygonShape box;
	box.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;

	b2Body* body = physics->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
	
	pEntity->SetBody(body);*/

	return true;
}

bool Engine::RemoveEntity(Entity* entity)
{
	auto iter = std::find(entitiesToRemove.begin(), entitiesToRemove.end(), entity);
	if (iter == entitiesToRemove.end())
		entitiesToRemove.push_back(entity);
	return true;
}

b2World* Engine::GetPhysics() const
{
	return physics.get();
}

b2Body* Engine::GetWorldBody() const
{
	return worldBody;
}

GameWorld* Engine::GetGameWorld() const
{
	return world;
}

void Engine::SetPlayer(Entity* player)
{
	this->player = player;
}

bool Engine::FindPath(const Point& start, const Point& end, std::deque<Point>* path) const
{
	struct TileNode
	{
		int G;
		int F;
		Point Location;
		int PathLength;
		TileNode* Parent;
		bool Closed;
		bool Open;
	};

	struct TileNodeComparer
	{
		bool operator()(const TileNode* a, const TileNode* b)
		{
			return a->F < b->F;
		}
	};

	int startH = 10 * (std::abs(end.x - start.x) + std::abs(end.y - start.y));

	TileNode* startNode = new TileNode;
	*startNode = { 0, startH, start, 0, nullptr };

	std::multiset<TileNode*, TileNodeComparer> openTiles;
	openTiles.insert(startNode);
	std::multiset<TileNode*, TileNodeComparer> closedTiles;

	std::unordered_map<Point, TileNode*, GameLocationHash> tiles;

	bool success = false;

	while (openTiles.size() > 0)
	{
		TileNode* lowestTile = *openTiles.begin();
		if (lowestTile->PathLength > 50 || lowestTile->Location == end)
		{
			TileNode* node = lowestTile;

			do
			{
				path->push_front(node->Location);
			} while ((node = node->Parent) != nullptr);

			success = true;

			break;
		}

		openTiles.erase(openTiles.begin());
		closedTiles.insert(lowestTile);

		const std::array<Point, 8> surround =
		{
			Point(-1, -1),
			Point(-1, 0),
			Point(-1, 1),
			Point(0, -1),
			Point(0, 1),
			Point(1, -1),
			Point(1, 0),
			Point(1, 1),
		};

		for (auto offset : surround)
		{
			Point location(lowestTile->Location);
			location.x += offset.x;
			location.y += offset.y;

			const Tile* tilesToCheck[3] = { 0 };

			tilesToCheck[0] = &world->GetTile(1, location.x, location.y);

			int newG = lowestTile->G;
			if (offset.x != 0 && offset.y != 0)
			{
				tilesToCheck[1] = &world->GetTile(1, location.x - offset.x, location.y);
				tilesToCheck[2] = &world->GetTile(1, location.x, location.y - offset.y);

				newG += 14;
			}
			else
				newG += 10;

			bool invalidMove = false;

			for (const Tile* tile : tilesToCheck)
			{
				if (tile != nullptr && (tile->Flags & TileFlags::BlockMovement) > 0)
				{
					invalidMove = true;
					break;
				}
			}

/*			class QueryCallback : public b2QueryCallback
			{
				bool foundFixture;
			public:
				QueryCallback()
				{
					foundFixture = false;
				}

				virtual bool ReportFixture(b2Fixture* fixture)
				{
					if (!fixture->IsSensor())
					{
						foundFixture = true;
						return false;
					}

					return true;
				}

				bool FoundFixture() const
				{
					return foundFixture;
				}
			};

			b2AABB box;
			box.lowerBound.Set(location.x, location.y);
			box.upperBound.Set(location.x + 0.99, location.y + 0.99);

			QueryCallback callback;

			physics->QueryAABB(&callback, box);

			if (callback.FoundFixture())
				continue;*/

			if (invalidMove)
				continue;

			int newF = newG + 10 * (std::abs(end.x - location.x) + std::abs(end.y - location.y));

			auto nodeIter = tiles.find(location);
			bool isClosed = false;
			bool isOpen = false;
			if (nodeIter != tiles.end())
			{
				isClosed = nodeIter->second->Closed;
				isOpen = nodeIter->second->Open;
			}

			if (isClosed && newF >= nodeIter->second->F)
				continue;

			if (!isOpen || newF < nodeIter->second->F)
			{
				TileNode* node;
				if (nodeIter != tiles.end())
					node = nodeIter->second;
				else
				{
					node = new TileNode;
					tiles[location] = node;
					node->Location = location;
				}
				node->Parent = lowestTile;
				node->Open = true;
				node->Closed = isClosed;
				node->PathLength = lowestTile->PathLength + 1;
				node->G = newG;
				node->F = newF;

				if (!isOpen)
					openTiles.insert(node);
			}
		}
	}

	for (TileNode* node : openTiles)
		delete node;

	for (TileNode* node : closedTiles)
		delete node;

	return success;
}