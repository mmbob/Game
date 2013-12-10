#include "SanityWeapon.h"

#include "Game.h"

SanityBullet::SanityBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime)
: Bullet(parent, pRenderer, pEngine, new TextureRenderObject(pRenderer), lifeTime)
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(0, 128, 16, 128 + 21));

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.angle = 0.0f;
	bodyDef.linearDamping = 0.0f;
	bodyDef.bullet = true;

	b2PolygonShape box;
	box.SetAsBox(renderer->PixelsToGameUnits(16) / 2.0f, renderer->PixelsToGameUnits(21) / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.isSensor = true;

	SetBody(engine->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);
}

SanityBullet::~SanityBullet()
{ }

std::wstring SanityBullet::GetTypeName() const
{
	return L"SanityBullet";
}

SanityWeapon::SanityWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: Weapon(parent, renderer, engine, fireCooldown)
{ }

bool SanityWeapon::Fire(const D3DXVECTOR2& start, float angle)
{
	const float bulletLifeTime = 1.0f;
	const float bulletSpeed = 5.0f;

	if (CanFire())
	{
		lastFireTime = g_pGameClient->GetGameTime();

		SanityBullet* bullet = new SanityBullet(this, renderer, engine, bulletLifeTime);

		D3DXVECTOR3 position;
		position.x = start.x;
		position.y = start.y;
		position.z = 0.4f;

		D3DXVECTOR2 velocity;
		velocity.x = bulletSpeed * std::cos(angle);
		velocity.y = -bulletSpeed * std::sin(angle);

		bullet->SetPosition(position);
		bullet->SetVelocity(velocity);
		//			bullet->SetRotation(atan2(diff.y, diff.x) + b2_pi / 2.0f);

		engine->AddEntity(bullet, EntityType::Dynamic);
		bullets.push_front(bullet);

		return true;
	}

	return false;
}

DoubleSanityWeapon::DoubleSanityWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: Weapon(parent, renderer, engine, fireCooldown)
{ }

bool DoubleSanityWeapon::Fire(const D3DXVECTOR2& start, float angle)
{
	const float bulletLifeTime = 1.0f;
	const float bulletSpeed = 5.0f;

	if (CanFire())
	{
		lastFireTime = g_pGameClient->GetGameTime();

		angle -= D3DX_PI / 4;

		SanityBullet* bulletsToCreate[2];
		for (SanityBullet*& bullet : bulletsToCreate)
		{
			bullet = new SanityBullet(this, renderer, engine, bulletLifeTime);
			D3DXVECTOR3 position;
			position.x = start.x;
			position.y = start.y;
			position.z = 0.4f;

			D3DXVECTOR2 velocity;
			velocity.x = bulletSpeed * std::cos(angle);
			velocity.y = -bulletSpeed * std::sin(angle);

			bullet->SetPosition(position);
			bullet->SetVelocity(velocity);
			//			bullet->SetRotation(atan2(diff.y, diff.x) + b2_pi / 2.0f);

			engine->AddEntity(bullet, EntityType::Dynamic);
			bullets.push_front(bullet);

			angle += D3DX_PI / 2;
		}

		return true;
	}

	return false;
}