#pragma once

#include <Box2D/Box2D.h>

#include "GameWorld.h"
#include "Entity.h"

struct EntityType
{
	enum Value
	{
		Static,
		Dynamic,
	};
};

class Engine
{
	GameWorld world;
	std::unique_ptr<b2World> physics;
	std::unique_ptr<b2Body> worldBody;
public:
	void Init();
	void UnInit();

	void Update();

	bool AddEntity(Entity* pEntity, EntityType::Value type);
};