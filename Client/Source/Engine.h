#pragma once

#include <list>

#include "GameWorld.h"
#include "Entity.h"
#include "Player.h"

using namespace std;

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
	list<Entity*> staticEntities;
	list<Entity*> dynamicEntities;
public:
	void Init();
	void UnInit();

	void Update();

	bool AddEntity(Entity* pEntity, EntityType::Value type);
};