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

bool Entity::GetFlags(int* pFlags) const
{
	*pFlags = flags;
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

bool Entity::SetFlags(int flags)
{
	this->flags = flags;
	return true;
}

RenderedEntity::RenderedEntity(Renderer* pRenderer)
{
	pRenderObject = new RenderObject(pRenderer);
	pRenderer->AddRenderObject(pRenderObject);
}

RenderedEntity::~RenderedEntity()
{

}

bool RenderedEntity::SetPosition(const D3DXVECTOR3& position)
{
	Entity::SetPosition(position);
	pRenderObject->SetPosition(position);
	return true;
}

bool RenderedEntity::SetRotation(const D3DXMATRIX& rotation)
{
	Entity::SetRotation(rotation);
	pRenderObject->SetRotation(rotation);
	return true;
}