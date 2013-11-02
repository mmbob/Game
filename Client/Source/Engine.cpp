#include "Engine.h"

void Engine::Init()
{

}

void Engine::UnInit()
{

}

void Engine::Update(float timeElapsed)
{
	for (Entity* pEntity : dynamicEntities)
	{
		D3DXVECTOR3 position;
		pEntity->GetPosition(&position);

		D3DXVECTOR2 velocity;
		pEntity->GetVelocity(&velocity);

		position.x += velocity.x * timeElapsed;
		position.y += velocity.y * timeElapsed;

		pEntity->SetPosition(position);
	}
}

bool Engine::AddEntity(Entity* pEntity, EntityType::Value type)
{
	switch (type)
	{
	case EntityType::Static:
		staticEntities.push_back(pEntity);
		break;

	case EntityType::Dynamic:
		dynamicEntities.push_back(pEntity);
		break;

	default:
		return false;
	}
	return true;
}