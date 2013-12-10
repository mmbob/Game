#include "Entity.h"

#include "Engine.h"
#include "Game.h"

IEntity::~IEntity()
{ }

Entity::Entity(Engine* engine)
: parent(engine), zIndex(0.0f), recentHealthChange(0), lastDamageTime(0.0f)
{
	userData.Type = BodyUserData::DataType::Entity;
	userData.Data.Entity = this;
}

bool Entity::GetPosition(D3DXVECTOR3* pPosition) const
{
	b2Vec2 position = body->GetPosition();
	pPosition->x = position.x;
	pPosition->y = position.y;
	pPosition->z = zIndex;
	return true;
}

bool Entity::GetRotation(float* pAngle) const
{
	*pAngle = body->GetAngle();
	return true;
}

bool Entity::GetVelocity(D3DXVECTOR2* pVelocity) const
{
	b2Vec2 velocity = body->GetLinearVelocity();
	pVelocity->x = velocity.x;
	pVelocity->y = velocity.y;
	return true;
}

bool Entity::GetFlags(int* pFlags) const
{
	*pFlags = flags;
	return true;
}

bool Entity::GetBody(b2Body** pBody) const
{
	*pBody = body;
	return true;
}

bool Entity::SetPosition(const D3DXVECTOR3& position)
{
	zIndex = position.z;
	b2Vec2 oldPosition = body->GetPosition();
	b2Vec2 newPosition = b2Vec2(position.x, position.y);
	if (oldPosition == newPosition)
		return true;

	b2Vec2 offset = b2Vec2_zero;
	body->SetTransform(newPosition, body->GetAngle());
	return true;
}

bool Entity::SetRotation(float angle)
{
	body->SetTransform(body->GetPosition(), angle);
	return true;
}

bool Entity::SetVelocity(const D3DXVECTOR2& velocity)
{
	body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
	return true;
}

bool Entity::SetFlags(int flags)
{
	this->flags = flags;
	return true;
}

bool Entity::SetBody(b2Body* body)
{
	assert(body != nullptr);
	this->body = body;
	body->SetUserData(&userData);
	return true;
}

void Entity::SetAlive(bool alive)
{
	body->SetActive(alive);
}

bool Entity::IsAlive() const
{
	return body->IsActive();
}

int Entity::GetHealth() const
{
	return health;
}

int Entity::Damage(int amount)
{
	health -= amount;

	float currentTime = g_pGameClient->GetGameTime();

	if (lastDamageTime + 2.0f > currentTime)
		recentHealthChange -= amount;
	else
		recentHealthChange = -amount;

	health = std::max<int>(0, health);

	lastDamageTime = currentTime;

	return health;
}

void Entity::Update(float)
{ }

std::wstring Entity::GetTypeName() const
{
	return L"Entity";
}

RenderedEntity::RenderedEntity(Renderer* pRenderer, Engine* pEngine, IRenderObject* renderObject)
: renderObject(renderObject), Entity(pEngine), renderer(pRenderer)
{
	pRenderer->AddRenderObject(renderObject);
}

RenderedEntity::~RenderedEntity()
{
	if (renderObject != nullptr && IsAlive())
		renderer->RemoveRenderObject(renderObject.get());
}

void RenderedEntity::UpdateRenderObject(bool updatePosition, bool updateRotation)
{
	if (!updatePosition && !updateRotation)
		return;

	RECT clip;
	reinterpret_cast<TextureRenderObject*>(renderObject.get())->GetTextureClip(&clip);

	D3DXVECTOR2 center2;
	center2.x = renderer->PixelsToGameUnits(clip.right - clip.left) / 2.0f;
	center2.y = renderer->PixelsToGameUnits(clip.bottom - clip.top) / 2.0f;

	if (updatePosition)
	{
		D3DXVECTOR3 position;
		GetPosition(&position);

		D3DXVECTOR3 center3;
		center3.x = center2.x;
		center3.y = center2.y;
		center3.z = 0.0f;
		renderObject->SetPosition(position - center3);
	}

	if (updateRotation)
	{
		float angle;
		GetRotation(&angle);

		D3DXMATRIX rotation;
		D3DXMatrixTransformation2D(&rotation, nullptr, 0.0f, nullptr, &center2, angle, nullptr);
		renderObject->SetRotation(rotation);
	}
}

bool RenderedEntity::SetPosition(const D3DXVECTOR3& position)
{
	Entity::SetPosition(position);
	UpdateRenderObject(true, false);
	return true;
}

bool RenderedEntity::SetRotation(float angle)
{
	Entity::SetRotation(angle);
	UpdateRenderObject(false, true);
	return true;
}

void RenderedEntity::SetAlive(bool alive)
{
	if (alive && !body->IsAwake())
	{
		renderer->AddRenderObject(renderObject.get());
	}
	else if (!alive && body->IsAwake())
	{
		renderer->RemoveRenderObject(renderObject.get());
	}
	Entity::SetAlive(alive);
}

std::wstring RenderedEntity::GetTypeName() const
{
	return L"RenderedEntity";
}