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
	GameWorld* world;
	std::unique_ptr<b2World> physics;
	b2Body* worldBody;
public:
	void Init(GameWorld* world);
	void UnInit();

	void Update(float timeElapsed);

	bool AddEntity(Entity* pEntity, EntityType::Value type);

	b2World* GetPhysics() const;
	b2Body* GetWorldBody() const;
};