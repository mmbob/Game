#include "Entity.h"

IEntity::~IEntity()
{ }

Entity::Entity()
	: position(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f)
{
	D3DXMatrixIdentity(&rotation);
}

bool Entity::GetPosition(D3DXVECTOR3* pPosition) const
{
	*pPosition = position;
	return true;
}

bool Entity::GetRotation(D3DXMATRIX* pRotation) const
{
	*pRotation = rotation;
	return true;
}

bool Entity::GetVelocity(D3DXVECTOR2* pVelocity) const
{
	*pVelocity = velocity;
	return true;
}

bool Entity::SetPosition(const D3DXVECTOR3& position)
{
	this->position = position;
	return true;
}

bool Entity::SetRotation(const D3DXMATRIX& rotation)
{
	this->rotation = rotation;
	return true;
}

bool Entity::SetVelocity(const D3DXVECTOR2& velocity)
{
	this->velocity = velocity;
	return true;
}

RenderedEntity::RenderedEntity(Renderer* pRenderer, std::unique_ptr<IRenderObject> renderObject)
: renderObject(renderObject.release())
{
	pRenderer->AddRenderObject(this->renderObject.get());
}

RenderedEntity::~RenderedEntity()
{

}

bool RenderedEntity::SetPosition(const D3DXVECTOR3& position)
{
	Entity::SetPosition(position);
	renderObject->SetPosition(position);
	return true;
}

bool RenderedEntity::SetRotation(const D3DXMATRIX& rotation)
{
	Entity::SetRotation(rotation);
	renderObject->SetRotation(rotation);
	return true;
}