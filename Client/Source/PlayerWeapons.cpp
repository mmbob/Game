#include "PlayerWeapons.h"

#include "Game.h"

LightningBullet::LightningBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime)
: Bullet(parent, pRenderer, pEngine, new TextureRenderObject(pRenderer), lifeTime)
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(64, 128, 128, 192));

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.fixedRotation = true;
	bodyDef.angle = 0.0f;
	bodyDef.linearDamping = 0.0f;
	bodyDef.bullet = true;

	b2PolygonShape box;
	box.SetAsBox(renderer->PixelsToGameUnits(64) / 2.0f, renderer->PixelsToGameUnits(64) / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.isSensor = true;

	SetBody(engine->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);
}

int LightningBullet::GetDamage() const
{
	return 3;
}

bool LightningBullet::IsDestroyedOnHit() const
{
	return false;
}

LightningBoltWeapon::LightningBoltWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: Weapon(parent, renderer, engine, fireCooldown)
{ }

bool LightningBoltWeapon::Fire(const D3DXVECTOR2& start, float angle)
{
	const float bulletLifeTime = 1.0f;
	const float bulletSpeed = 5.0f;

	if (CanFire())
	{
		lastFireTime = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();

		LightningBullet* bullet = new LightningBullet(this, renderer, engine, bulletLifeTime);

		D3DXVECTOR3 position(start.x, start.y, 0.4f);
		position.x += boltDistance * std::cos(angle);
		position.y -= boltDistance * std::sin(angle);

		D3DXVECTOR2 velocity(0.0f, 0.0f);

		bullet->SetPosition(position);
		bullet->SetVelocity(velocity);
		//			bullet->SetRotation(atan2(diff.y, diff.x) + b2_pi / 2.0f);

		engine->AddEntity(bullet, EntityType::Dynamic);
		bullets.push_front(bullet);

		return true;
	}

	return false;
}

void LightningBoltWeapon::SetBoltDistance(float distance)
{
	boltDistance = distance;
}

FlameBoltBullet::FlameBoltBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime)
: Bullet(parent, pRenderer, pEngine, new TextureRenderObject(pRenderer), lifeTime)
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(128, 128, 128 + 32, 128 + 32));

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.angle = 0.0f;
	bodyDef.linearDamping = 0.0f;
	bodyDef.bullet = true;

	b2PolygonShape box;
	box.SetAsBox(renderer->PixelsToGameUnits(32) / 2.0f, renderer->PixelsToGameUnits(32) / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.isSensor = true;

	SetBody(engine->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);
}

int FlameBoltBullet::GetDamage() const
{
	return 2;
}

bool FlameBoltBullet::IsDestroyedOnHit() const
{
	return false;
}

FlameBoltWeapon::FlameBoltWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: Weapon(parent, renderer, engine, fireCooldown)
{ }

bool FlameBoltWeapon::Fire(const D3DXVECTOR2& start, float angle)
{
	const float bulletLifeTime = 1.0f;
	const float bulletSpeed = 5.0f;

	if (CanFire())
	{
		lastFireTime = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();

		FlameBoltBullet* bullet = new FlameBoltBullet(this, renderer, engine, bulletLifeTime);

		D3DXVECTOR3 position(start.x, start.y, 0.4f);

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

ShockwaveBullet::ShockwaveBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine, float lifeTime, Direction direction)
: Bullet(parent, pRenderer, pEngine, new TextureRenderObject(pRenderer), lifeTime)
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	Rect clips[4] = 
	{
		Rect(230, 128, 230 + 27, 128 + 30),
		Rect(192, 128, 192 + 27, 128 + 30),
		Rect(192, 163, 192 + 27, 163 + 30),
		Rect(230, 163, 230 + 27, 163 + 30),
	};

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(clips[direction]);

	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	bodyDef.fixedRotation = true;
	bodyDef.angle = 0.0f;
	bodyDef.linearDamping = 0.0f;
	bodyDef.bullet = true;

	b2PolygonShape box;
	box.SetAsBox(renderer->PixelsToGameUnits(27) / 2.0f, renderer->PixelsToGameUnits(30) / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	fixtureDef.isSensor = true;

	SetBody(engine->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);
}

int ShockwaveBullet::GetDamage() const
{
	return 3;
}

bool ShockwaveBullet::IsDestroyedOnHit() const
{
	return false;
}

ShockwaveWeapon::ShockwaveWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: Weapon(parent, renderer, engine, fireCooldown)
{ }

bool ShockwaveWeapon::Fire(const D3DXVECTOR2& start, float angle)
{
	const float bulletLifeTime = 1.0f;
	const float bulletSpeed = 5.0f;

	if (CanFire())
	{
		lastFireTime = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();

		ShockwaveBullet::Direction direction = ShockwaveBullet::UpRight;
		angle = D3DX_PI / 4.0f;

		for (int i = 0; i < 4; ++i)
		{
			ShockwaveBullet* bullet = new ShockwaveBullet(this, renderer, engine, bulletLifeTime, direction);

			D3DXVECTOR3 position(start.x, start.y, 0.4f);

			D3DXVECTOR2 velocity;
			velocity.x = bulletSpeed * std::cos(angle);
			velocity.y = -bulletSpeed * std::sin(angle);

			bullet->SetPosition(position);
			bullet->SetVelocity(velocity);
//			bullet->SetRotation(angle);

			engine->AddEntity(bullet, EntityType::Dynamic);
			bullets.push_front(bullet);

			angle += D3DX_PI / 2.0f;
			direction = (ShockwaveBullet::Direction) (int(direction) + 1);
		}

		return true;
	}

	return false;
}