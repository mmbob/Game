#pragma once

#include <list>

#include "GameWorld.h"
#include "Entity.h"

using namespace std;

class Engine
{
	GameWorld world;
	list<Entity*> entities;
public:
	void Init();
	void UnInit();


};