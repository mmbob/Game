#pragma once

#include <Box2D/Box2D.h>
#include <list>

#include "GameWorld.h"

class Entity;

struct EntityType
{
	enum Value
	{
		Static,
		Dynamic,
	};
};

struct BodyUserData
{
	struct DataType
	{
		enum Value
		{
			Entity,
			Tile,
			Other,
		};
	};

	DataType::Value Type;

	union
	{
		Entity* Entity;
		const Tile* Tile;
		void* Other;
	} Data;
};

class Engine
{
	GameWorld* world;
	std::unique_ptr<b2World> physics;
	b2Body* worldBody;
	Entity* player;

	std::list<Entity*> dynamicEntities;
	std::list<Entity*> entitiesToRemove;
public:
	Engine();

	void Init(GameWorld* world);
	void UnInit();

	void Update(float timeElapsed);

	bool AddEntity(Entity* entity, EntityType::Value type);
	bool RemoveEntity(Entity* entity);

	b2World* GetPhysics() const;
	b2Body* GetWorldBody() const;

	GameWorld* GetGameWorld() const;

	void SetPlayer(Entity* player);

	bool FindPath(const Point& start, const Point& end, std::deque<Point>* path) const;
};