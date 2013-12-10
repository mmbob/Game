#include "Player.h"

#include <algorithm>

#include "Game.h"
#include "Engine.h"
#include "SanityCrystal.h"
#include "Enemy.h"
#include "SanityWeapon.h"

Player::Player(Renderer* pRenderer, Engine* pEngine)
: RenderedEntity(pRenderer, pEngine, new TextureRenderObject(pRenderer))
{
	health = 20;

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(64, 0, 64 + 50, 50));

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.angle = 0.0f;
	bodyDef.linearDamping = 10.0f;

	b2PolygonShape box;
	box.SetAsBox(renderer->PixelsToGameUnits(50) / 2.0f, renderer->PixelsToGameUnits(50) / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;

	SetBody(parent->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);

	WeaponInfo sanityInfo;
	sanityInfo.SanityCost = 1;
	sanityInfo.Weapon = new SanityWeapon(this, pRenderer, pEngine, 0.5f);

	WeaponInfo sanity2Info;
	sanity2Info.SanityCost = 2;
	sanity2Info.Weapon = new DoubleSanityWeapon(this, pRenderer, pEngine, 0.7f);

	WeaponInfo lightningInfo;
	lightningInfo.SanityCost = 3;
	lightningInfo.Weapon = new LightningBoltWeapon(this, pRenderer, pEngine, 1.0f);

	WeaponInfo flameInfo;
	flameInfo.SanityCost = 3;
	flameInfo.Weapon = new FlameBoltWeapon(this, pRenderer, pEngine, 1.0f);

	WeaponInfo shockwaveInfo;
	shockwaveInfo.SanityCost = 8;
	shockwaveInfo.Weapon = new ShockwaveWeapon(this, pRenderer, pEngine, 2.0f);

	weapons.push_back(sanityInfo);
	weapons.push_back(sanity2Info);
	weapons.push_back(lightningInfo);
	weapons.push_back(flameInfo);
	weapons.push_back(shockwaveInfo);

	ChangeWeapon(0);

	reinterpret_cast<LightningBoltWeapon*>(lightningInfo.Weapon)->SetBoltDistance(3.0f);
}

Player::~Player()
{
	for (WeaponInfo weapon : weapons)
		delete weapon.Weapon;
}

void Player::HandleCollisions()
{
	for (b2ContactEdge* contacts = body->GetContactList(); contacts != nullptr;)
	{
		b2Contact* contact = contacts->contact;
		contacts = contacts->next;

		if (!contact->IsTouching())
			continue;

		auto userDatas =
		{
			reinterpret_cast<BodyUserData*>(contact->GetFixtureA()->GetBody()->GetUserData()),
			reinterpret_cast<BodyUserData*>(contact->GetFixtureB()->GetBody()->GetUserData()),
		};

		for (auto userData : userDatas)
		{
			if (userData != nullptr && userData->Type == BodyUserData::DataType::Entity)
			{
				Entity* entity = userData->Data.Entity;
				if (entity->GetTypeName() == L"SanityCrystal")
				{
					health++;

					entity->SetAlive(false);
				}
				else if (entity->GetTypeName().find(L"Bullet") != std::wstring::npos)
				{
					Bullet* bullet = reinterpret_cast<Bullet*>(entity);

					b2Body* bulletParentBody;
					bullet->GetWeapon()->GetParent()->GetBody(&bulletParentBody);
					if (bulletParentBody != body)
						Damage(bullet->GetDamage());
				}
			}
		}
	}
}

void Player::Update(float timeElapsed)
{
	DirectXManager* pDirectX = renderer->GetDirectX();

	UpdateRenderObject(true, false);

	HandleCollisions();

	bool left = pDirectX->IsKeyPressed(DIK_A);
	bool up = pDirectX->IsKeyPressed(DIK_W);
	bool right = pDirectX->IsKeyPressed(DIK_D);
	bool down = pDirectX->IsKeyPressed(DIK_S);

	b2Vec2 force = b2Vec2_zero;
	const float accelValue = 50.0f;

	if (left && !right)
		force.x = -accelValue;
	else if (right && !left)
		force.x = accelValue;
	if (up && !down)
		force.y = -accelValue;
	else if (down && !up)
		force.y = accelValue;

	body->ApplyForceToCenter(force);

	if (pDirectX->IsKeyPressed(DIK_1))
		ChangeWeapon(0);
	else if (pDirectX->IsKeyPressed(DIK_2))
		ChangeWeapon(1);
	else if (pDirectX->IsKeyPressed(DIK_3))
		ChangeWeapon(2);

	if (health > weapons[currentWeaponIndex].SanityCost)	// Can only shoot if we have sanity
	{
		float bulletAngle = 1.0f;
		if (pDirectX->IsKeyPressed(DIK_LEFT))
			bulletAngle = D3DX_PI;
		else if (pDirectX->IsKeyPressed(DIK_UP))
			bulletAngle = D3DX_PI / 2.0f;
		else if (pDirectX->IsKeyPressed(DIK_RIGHT))
			bulletAngle = 0.0f;
		else if (pDirectX->IsKeyPressed(DIK_DOWN))
			bulletAngle = -D3DX_PI / 2.0f;

		if (bulletAngle != 1.0f)	// 5.0f is the "no button pressed" value
		{
			D3DXVECTOR3 position;
			GetPosition(&position);

			D3DXVECTOR2 bulletPosition(position.x, position.y);
			bool success = currentWeapon->Fire(bulletPosition, bulletAngle);
			if (success)
				health -= weapons[currentWeaponIndex].SanityCost;
		}
	}

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	float timeSinceLastDamage = g_pGameClient->GetGameTime() - lastDamageTime;

	int flashValue = 255;
	if (timeSinceLastDamage < 2.0f)
		flashValue = 128 + int(127 * std::sin(timeSinceLastDamage * 4 * D3DX_PI));

	textureObject->SetColor(D3DCOLOR_ARGB(255, 255, flashValue, flashValue));

	std::vector<std::pair<Bullet*, BodyUserData*> > bulletCollisions;

	for (WeaponInfo& weapon : weapons)
	{
		weapon.Weapon->Update(timeElapsed);
		auto partialCollisions = weapon.Weapon->GetBulletCollisions();
		bulletCollisions.insert(bulletCollisions.end(), partialCollisions.begin(), partialCollisions.end());
	}

	for (auto bulletPair : bulletCollisions)
	{
		Bullet* bullet = bulletPair.first;
		BodyUserData* userData = bulletPair.second;

		if (userData->Type == BodyUserData::DataType::Entity)
		{
			Entity* entity = userData->Data.Entity;

			if (entity->GetTypeName().find(L"Enemy") != std::wstring::npos)
			{
				if (bullet->IsDestroyedOnHit())
					reinterpret_cast<Weapon*>(bullet->GetWeapon())->RemoveBullet(bullet);

				if (!bullet->HasHitEntity(entity))
				{
					bullet->AddHitEntity(entity);

					Enemy* enemy = reinterpret_cast<Enemy*>(entity);
					enemy->Damage(bullet->GetDamage());
				}
			}
		}
		else if (userData->Type == BodyUserData::DataType::Tile)
		{
			if (bullet->IsDestroyedOnHit())
				reinterpret_cast<Weapon*>(bullet->GetWeapon())->RemoveBullet(bullet);
		}
	}
}

int Player::GetSanity() const
{
	return health;
}

void Player::ChangeWeapon(int index)
{
	currentWeaponIndex = index;
	currentWeapon = weapons[index].Weapon;
}

int Player::GetCurrentWeaponCost() const
{
	return weapons[currentWeaponIndex].SanityCost;
}

std::wstring Player::GetTypeName() const
{
	return L"Player";
}