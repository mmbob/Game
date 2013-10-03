#include "Entity.h"

Entity::~Entity()
{ }

RenderedEntity::RenderedEntity(Renderer* pRenderer)
{
	pRenderObject = pRenderer->CreateRenderObject();
}

RenderedEntity::~RenderedEntity()
{

}