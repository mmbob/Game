#include "EnemyWeapons.h"

#include "Game.h"

SpiderMeleeBullet::SpiderMeleeBullet(IWeapon* parent, Renderer* pRenderer, Engine* pEngine)
: Bullet(parent, pRenderer, pEngine, new TextureRenderObject(pRenderer), 0.10f)
{
	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(0, 0, 0, 0));

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
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

int SpiderMeleeBullet::GetDamage() const
{
	return 2;
}

SpiderMeleeWeapon::SpiderMeleeWeapon(Entity* parent, Renderer* renderer, Engine* engine, float fireCooldown)
: Weapon(parent, renderer, engine, fireCooldown)
{ }

bool SpiderMeleeWeapon::Fire(const D3DXVECTOR2& start, float angle)
{
	const float attackDistance = 0.25f;

	if (CanFire())
	{
		lastFireTime = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();

		SpiderMeleeBullet* bullet = new SpiderMeleeBullet(this, renderer, engine);

		D3DXVECTOR3 position(start.x, start.y, 0.4f);
		position.x += attackDistance * std::cos(angle);
		position.y -= attackDistance * std::sin(angle);

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