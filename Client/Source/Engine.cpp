#include "Engine.h"

void Engine::Init()
{
	physics = std::unique_ptr<b2World>(new b2World(b2Vec2_zero));

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.position.SetZero();
	
	worldBody = std::unique_ptr<b2Body>(physics->CreateBody(&bodyDef));
}

void Engine::UnInit()
{
	worldBody.release();

	physics.release();
}

void Engine::Update()
{
	const float timeStep = 1.0f / 60.0f;		// 60 steps per second
	const int velocityIterations = 10;
	const int positionIterations = 10;
	physics->Step(timeStep, velocityIterations, positionIterations);

	physics->ClearForces();
/*	for (Entity* pEntity : dynamicEntities)
	{
		D3DXVECTOR3 position;
		pEntity->GetPosition(&position);

		D3DXVECTOR2 velocity;
		pEntity->GetVelocity(&velocity);

		position.x += velocity.x;
		position.y += velocity.y;

		pEntity->SetPosition(position);
	}*/
}

bool Engine::AddEntity(Entity* pEntity, EntityType::Value type)
{
	b2BodyDef bodyDef;
	switch (type)
	{
	case EntityType::Static:
		bodyDef.type = b2_staticBody;
		break;

	case EntityType::Dynamic:
		bodyDef.type = b2_dynamicBody;
		break;

	default:
		return false;
	}
	b2PolygonShape box;
	box.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;

	b2Body* body = physics->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);

	return true;
}