#include "Weapon.h"

#include "Game.h"

IBullet::IBullet(Renderer* pRenderer, Engine* pEngine, IRenderObject* pRenderObject)
: RenderedEntity(pRenderer, pEngine, pRenderObject)
{ }

IBullet::~IBullet()
{ }

Bullet::Bullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, IRenderObject* pRenderObject, float lifeTime)
: IBullet(pRenderer, pEngine, pRenderObject), parent(parent), renderer(pRenderer), engine(pEngine), endOfLifeTime(g_pGameClient->GetGameTime() + lifeTime)
{ }

void Bullet::Update(float)
{
	b2Vec2 physicsPosition = body->GetPosition();
	D3DXVECTOR3 position(physicsPosition.x, physicsPosition.y, zIndex);
	SetPosition(position);
}

IWeapon* Bullet::GetWeapon() const
{
	return reinterpret_cast<IWeapon*>(parent);
}

void Bullet::AddHitEntity(IEntity* entity)
{
	hitEntities.insert(entity);
}

bool Bullet::HasHitEntity(IEntity* entity) const
{
	return hitEntities.find(entity) != hitEntities.end();
}

int Bullet::GetDamage() const
{
	return 1;
}

bool Bullet::IsLifeOver() const
{
	return endOfLifeTime < g_pGameClient->GetGameTime();
}

bool Bullet::IsDestroyedOnHit() const
{
	return true;
}

std::wstring Bullet::GetTypeName() const
{
	return L"Bullet";
}

Weapon::Weapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: parent(parent), renderer(renderer), engine(engine), fireCooldown(fireCooldown), lastFireTime(0.0f)
{ }

IWeapon::~IWeapon()
{ }

void Weapon::Update(float)
{
	bulletCollisions.clear();

	std::vector<std::list<Bullet*>::iterator> bulletsToDelete;
	for (auto iter = bullets.begin(); iter != bullets.end(); ++iter)
	{
		Bullet* bullet = reinterpret_cast<Bullet*>(*iter);

		if (bullet->IsLifeOver())
		{
			bulletsToDelete.push_back(iter);
			continue;
		}

		b2Body* bulletBody;
		bullet->GetBody(&bulletBody);

		for (b2ContactEdge* contacts = bulletBody->GetContactList(); contacts != nullptr; contacts = contacts->next)
		{
			if (!contacts->contact->IsTouching())
				continue;

			b2Fixture* otherFixture;

			if (contacts->contact->GetFixtureA()->GetBody() == bulletBody)
				otherFixture = contacts->contact->GetFixtureB();
			else
				otherFixture = contacts->contact->GetFixtureA();

			if (otherFixture->IsSensor())
				continue;

			BodyUserData* otherUserData = reinterpret_cast<BodyUserData*>(otherFixture->GetBody()->GetUserData());

			if (otherUserData != nullptr)
				bulletCollisions.push_back(std::make_pair(bullet, otherUserData));
		}
	}

	for (auto iter : bulletsToDelete)
	{
		engine->RemoveEntity(*iter);
		bullets.erase(iter);
	}
}

const std::vector<std::pair<Bullet*, BodyUserData*> >& Weapon::GetBulletCollisions() const
{
	return bulletCollisions;
}

void Weapon::RemoveBullet(Bullet* bullet)
{
	auto iter = std::find(bullets.begin(), bullets.end(), bullet);
	if (iter != bullets.end())
	{
		bullets.erase(iter);
		engine->RemoveEntity(bullet);
	}
}

void Weapon::SetFireCooldown(float fireCooldown)
{
	this->fireCooldown = fireCooldown;
}

float Weapon::GetFireCooldown() const
{
	return fireCooldown;
}

bool Weapon::CanFire() const
{
	return lastFireTime + fireCooldown < g_pGameClient->GetGameTime();
}

Entity* Weapon::GetParent() const
{
	return parent;
}