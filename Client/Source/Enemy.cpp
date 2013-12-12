#include "Enemy.h"

#include "Game.h"
#include "GameState.h"
#include "SanityCrystal.h"
#include "EnemyWeapons.h"
#include "Player.h"

class LineRenderObject : public RenderObject
{
	const std::deque<Point>* path;
public:
	LineRenderObject::LineRenderObject(Renderer* pParent)
		: RenderObject(pParent)
	{ }

	virtual void Render(LPD3DXSPRITE sprite) const
	{
		auto line = parent->GetDirectX()->GetLine();

		bool onScreen = false;
		int index = 0;
		D3DXVECTOR2 vertices[52];
		for (Point location : *path)
		{
			onScreen |= parent->WorldToScreen(D3DXVECTOR3(location.x, location.y, 1.0f), &vertices[index]);
			index++;
		}

		if (onScreen)
		{
			line->Begin();
			line->Draw(vertices, index, 0x00000000);
			line->End();
		}
	}

	virtual RenderObjectType::Value GetType() const
	{
		return RenderObjectType::Custom;
	}

	void SetDrawPath(const std::deque<Point>& path)
	{
		this->path = &path;
	}
};

Enemy::Enemy(Renderer* pRenderer, Engine* pEngine)
: RenderedEntity(pRenderer, pEngine, new TextureRenderObject(pRenderer)), lastSuccessfulMove(0.0f), 
lastSawPlayerTime(0.0f), textureAlpha(255), recentDamageAlpha(0)
{
	pathRender.reset(new LineRenderObject(pRenderer));
	pathRender->SetDrawPath(movePath);

	damageRender.reset(new TextRenderObject(pRenderer));
	damageRender->SetFont(renderer->GetFont(L"UISmall"));
}

Enemy::~Enemy()
{
	renderer->RemoveRenderObject(damageRender.get());
	for (Entity* drop : drops)
	{
		parent->RemoveEntity(drop);
	}
}

void Enemy::CreateBody(int width, int height, float density)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	bodyDef.angle = 0.0f;
	bodyDef.linearDamping = 0.0f;

	float x = 1.0f * float(rand() % 3 - 1);
	float y = 1.0f * float(rand() % 3 - 1);
	if (x == 0.0f && y == 0.0f)
	{
		if (rand() % 2 == 0)
			x = 2.0f * (float(rand() % 2) - 0.5f);
		else
			y = 2.0f * (float(rand() % 2) - 0.5f);
	}
	bodyDef.linearVelocity = b2Vec2(x, y);

	b2PolygonShape box;
	box.SetAsBox(renderer->PixelsToGameUnits(width) / 2.0f, renderer->PixelsToGameUnits(width) / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = density;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;

	SetBody(parent->GetPhysics()->CreateBody(&bodyDef));
	body->CreateFixture(&fixtureDef);
}

void Enemy::Update(float timeElapsed)
{
	currentWeapon->Update(timeElapsed);

	auto bulletCollisions = currentWeapon->GetBulletCollisions();

	for (auto bulletPair : bulletCollisions)
	{
		Bullet* bullet = bulletPair.first;
		BodyUserData* userData = bulletPair.second;

		if (userData->Type == BodyUserData::DataType::Entity)
		{
			Entity* entity = userData->Data.Entity;

			if (entity->GetTypeName() == L"Player")
			{
				currentWeapon->RemoveBullet(bullet);
				break;
			}
		}
		else if (userData->Type == BodyUserData::DataType::Tile)
		{
			if (bullet->IsDestroyedOnHit())
				reinterpret_cast<Weapon*>(bullet->GetWeapon())->RemoveBullet(bullet);
		}
	}

	if (!body->IsActive())
		return;

	b2Vec2 position = body->GetPosition();

	b2Body* playerBody;
	reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetPlayer()->GetBody(&playerBody);

	b2Vec2 playerPosition = playerBody->GetPosition();
	float playerDistance = (playerPosition - position).Length();

	UpdateRenderObject(true, false);

	if (!movePath.empty())
	{
		if (lastSuccessfulMove + 5.0f < reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime())
			CancelMove();
		else
		{
			Point nextLocation = movePath.front();
			if ((position - b2Vec2(float(nextLocation.x), float(nextLocation.y))).Length() < 0.05f)
			{
				movePath.pop_front();

				lastSuccessfulMove = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();
			}

			if (!movePath.empty())
			{
				nextLocation = movePath.front();

				b2Vec2 moveVelocity(nextLocation.x - position.x, nextLocation.y - position.y);
				moveVelocity.Normalize();
				moveVelocity *= maxSpeed;

				SetVelocity(D3DXVECTOR2(moveVelocity.x, moveVelocity.y));
			}
			else
				CancelMove();
		}
	}

	for (b2ContactEdge* contacts = body->GetContactList(); contacts != nullptr; contacts = contacts->next)
	{
		if (!contacts->contact->IsTouching())
			continue;
		if (contacts->contact->GetFixtureA()->IsSensor() || contacts->contact->GetFixtureB()->IsSensor())
			continue;

		float x = 1.0f * float(rand() % 3 - 1);
		float y = 1.0f * float(rand() % 3 - 1);
		if (x == 0.0f && y == 0.0f)
		{
			if (rand() % 2 == 0)
				x = 2.0f * (float(rand() % 2) - 0.5f);
			else
				y = 2.0f * (float(rand() % 2) - 0.5f);
		}

		x *= maxSpeed;
		y *= maxSpeed;

		SetVelocity(D3DXVECTOR2(x, y));

		CancelMove();

		break;
	}


	D3DXVECTOR3 position3;
	GetPosition(&position3);

	D3DXVECTOR2 screen;
	if (recentDamageAlpha > 0)
	{
		if (renderer->WorldToScreen(position3, &screen))
		{
			RECT gameArea = renderer->GetGameArea();
			damageRender->SetRect(Rect(screen.x, screen.y, gameArea.right, gameArea.bottom));
		}
	}

	const bool isDayTime = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->IsDayTime();

	const float playerSeeDistance = isDayTime ? 5.0f : 4.2f;
	const float enemySeeDistance = isDayTime ? 4.2f : 5.0f;
	const float maxSeeDistance = 5.0f;

	bool seesPlayer = false;

	if (playerDistance < maxSeeDistance)
	{
		PlayerSightRayCast playerSight(parent, this);
		parent->GetPhysics()->RayCast(&playerSight, position, playerPosition);

		seesPlayer = playerSight.SeesPlayer;
	}

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	int oldTextureAlpha = textureAlpha;

	if (seesPlayer)
	{
		if (playerDistance < playerSeeDistance)
		{
			if (textureAlpha == 0)
				renderer->AddRenderObject(textureObject);

			textureAlpha += (timeElapsed * 1000.0f) + 1;
			textureAlpha = std::min<int>(255, textureAlpha);
		}

		if (playerDistance < enemySeeDistance)
		{
			lastSawPlayerTime = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();

			Point start(int(position.x + 0.5f), int(position.y + 0.5f));
			Point end(int(playerPosition.x + 0.5f), int(playerPosition.y + 0.5f));

			if (movePath.empty() || movePath.back() != end)
			{
				movePath.clear();

				std::deque<Point> newMovePath;
				parent->FindPath(start, end, &newMovePath);

				if (movePath.empty() || newMovePath.front() != movePath.front())
					newMovePath.pop_front();

				movePath = newMovePath;

				lastSuccessfulMove = reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime();
			}

			if (currentWeapon->CanFire())
			{
				D3DXVECTOR2 bulletStart(position.x, position.y);
				float bulletAngle = -std::atan2(playerPosition.y - position.y, playerPosition.x - position.x);

				currentWeapon->Fire(bulletStart, bulletAngle);
			}
		}
	}
	else if (textureAlpha > 0)
	{
		textureAlpha -= int(timeElapsed * 100.0f) + 1;

		if (textureAlpha <= 0)
		{
			textureAlpha = 0;
			renderer->RemoveRenderObject(textureObject);
		}
	}

	int flashValue = 255;
	if (recentDamageAlpha > 0)
		flashValue = 128 + 127 * std::sin((reinterpret_cast<InGameState*>(g_pGameClient->GetChild())->GetGameTime() - lastDamageTime) * 4 * D3DX_PI);

	textureObject->SetColor(D3DCOLOR_ARGB(textureAlpha, 255, flashValue, flashValue));

	if (recentDamageAlpha > 0)
	{
		recentDamageAlpha -= int(timeElapsed * 100.0f) + 1;
		
		if (recentDamageAlpha > 0)
			damageRender->SetColor(D3DCOLOR_ARGB(recentDamageAlpha * textureAlpha / 256, 255, 255, 255));
		else
		{
			recentDamageAlpha = 0;
			renderer->RemoveRenderObject(damageRender.get());
		}
	}
}

void Enemy::CancelMove()
{
	movePath.clear();
//	renderer->RemoveRenderObject(pathRender);
}

int Enemy::Damage(int amount)
{
	Entity::Damage(amount);

	D3DXVECTOR3 position;
	GetPosition(&position);

	if (health <= 0)
	{
		SetAlive(false);

		for (int i = 0; i < sanityDropped; ++i)
		{
			SanityCrystal* crystal = new SanityCrystal(renderer, parent);
			crystal->SetPosition(position + D3DXVECTOR3((float(rand()) / RAND_MAX) - 0.5f, (float(rand()) / RAND_MAX) - 0.5f, 0.0f));

			parent->AddEntity(crystal, EntityType::Static);
			drops.push_back(crystal);
		}
	}
	else
	{
		D3DXVECTOR2 screen;
		if (renderer->WorldToScreen(position, &screen))
		{
			std::wstring damageText(L"-");
			damageText += '0' - recentHealthChange;

			damageRender->SetFormat(DT_SINGLELINE);
			damageRender->SetText(damageText);

			if (recentDamageAlpha == 0)
				renderer->AddRenderObject(damageRender.get());

			recentDamageAlpha = 255;
		}
	}

	return health;
}

int Enemy::GetSanityDropped() const
{
	return sanityDropped;
}

void Enemy::SetAlive(bool alive)
{
	RenderedEntity::SetAlive(alive);

	if (!alive)
	{
		renderer->RemoveRenderObject(renderObject.get());
		renderer->RemoveRenderObject(damageRender.get());
		renderer->RemoveRenderObject(pathRender.get());
	}
}

std::wstring Enemy::GetTypeName() const
{
	return L"Enemy";
}

Enemy::PlayerSightRayCast::PlayerSightRayCast(Engine* engine, Enemy* parent)
: engine(engine), parent(parent), SeesPlayer(true)
{ }

float32 Enemy::PlayerSightRayCast::ReportFixture(b2Fixture* fixture, const b2Vec2&, const b2Vec2&, float32)
{
	auto userData = reinterpret_cast<BodyUserData*>(fixture->GetBody()->GetUserData());

	if (userData != nullptr)
	{
		if (userData->Type == BodyUserData::DataType::Entity)
		{
			if (fixture->IsSensor())
				return -1.0f;

			Entity* entity = userData->Data.Entity;
			if (entity == parent)
				return -1.0f;
			else if (entity->GetTypeName() == L"Player")
				return -1.0f;
		}
		else if (userData->Type == BodyUserData::DataType::Tile)
		{
			const Tile* tile = userData->Data.Tile;
			if ((tile->Flags & TileFlags::BlockSight) == 0)
				return -1.0f;
		}
	}

	SeesPlayer = false;

	return 0.0f;
}

CloudEnemy::CloudEnemy(Renderer* pRenderer, Engine* pEngine)
: Enemy(pRenderer, pEngine)
{
	maxSpeed = 1.2f;
	health = 4;
	sanityDropped = 8;
	currentWeapon.reset(new SanityWeapon(this, pRenderer, pEngine, 2.0f));

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(67, 70, 67 + 58, 70 + 53));

	CreateBody(58, 53, 0.25f);
}

std::wstring CloudEnemy::GetTypeName() const
{
	return L"CloudEnemy";
}

SpiderEnemy::SpiderEnemy(Renderer* pRenderer, Engine* pEngine)
: Enemy(pRenderer, pEngine)
{
	maxSpeed = 1.5f;
	health = 3;
	sanityDropped = 10;
	currentWeapon.reset(new SpiderMeleeWeapon(this, pRenderer, pEngine, 0.75f));

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(131, 68, 131 + 30, 68 + 30));

	CreateBody(30, 30, 0.25f);
}

std::wstring SpiderEnemy::GetTypeName() const
{
	return L"SpiderEnemy";
}

PuddleEnemy::PuddleEnemy(Renderer* pRenderer, Engine* pEngine)
: Enemy(pRenderer, pEngine)
{
	maxSpeed = 0.05f;
	health = 10;
	sanityDropped = 15;
	currentWeapon.reset(new SanityWeapon(this, pRenderer, pEngine, 5.0f));

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(195, 66, 195 + 58, 66 + 61));

	CreateBody(58, 61, 4.0f);
}

std::wstring PuddleEnemy::GetTypeName() const
{
	return L"PuddleEnemy";
}

GhostEnemy::GhostEnemy(Renderer* pRenderer, Engine* pEngine)
: Enemy(pRenderer, pEngine)
{
	maxSpeed = 0.8f;
	health = 2;
	sanityDropped = 4;
	currentWeapon.reset(new SanityWeapon(this, pRenderer, pEngine, 1.5f));

	auto textureObject = reinterpret_cast<TextureRenderObject*>(renderObject.get());

	textureObject->SetTextureName(L"Entities1");
	textureObject->SetTextureClip(Rect(262, 67, 262 + 55, 67 + 58));

	CreateBody(55, 58, 0.1f);
}

std::wstring GhostEnemy::GetTypeName() const
{
	return L"GhostEnemy";
}