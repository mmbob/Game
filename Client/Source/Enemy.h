#pragma once

#include "Entity.h"

class Weapon;
class LineRenderObject;

class Enemy : public RenderedEntity
{
protected:
	class PlayerSightRayCast : public b2RayCastCallback
	{
		Engine* engine;
		Enemy* parent;
	public:
		PlayerSightRayCast(Engine* engine, Enemy* parent);

		virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);

		bool SeesPlayer;
	};

	float maxSpeed;
	int sanityDropped;

	std::unique_ptr<Weapon> currentWeapon;

	std::deque<Point> movePath;
	float lastSuccessfulMove;
	float lastSawPlayerTime;

	int textureAlpha;

	std::unique_ptr<LineRenderObject> pathRender;
	std::unique_ptr<TextRenderObject> damageRender;
	int recentDamageAlpha;

	void CreateBody(int width, int height, float density);
	void CancelMove();
public:
	Enemy(Renderer* pRenderer, Engine* pEngine);
	~Enemy();

	void Update(float timeElapsed);

	virtual int Damage(int amount);

	int GetSanityDropped() const;

	virtual void SetAlive(bool alive);

	virtual std::wstring GetTypeName() const;
};

class CloudEnemy : public Enemy
{
public:
	CloudEnemy(Renderer* pRenderer, Engine* pEngine);

	virtual std::wstring GetTypeName() const;
};

class SpiderEnemy : public Enemy
{
public:
	SpiderEnemy(Renderer* pRenderer, Engine* pEngine);

	virtual std::wstring GetTypeName() const;
};

class PuddleEnemy : public Enemy
{
public:
	PuddleEnemy(Renderer* pRenderer, Engine* pEngine);

	virtual std::wstring GetTypeName() const;
};

class GhostEnemy : public Enemy
{
public:
	GhostEnemy(Renderer* pRenderer, Engine* pEngine);

	virtual std::wstring GetTypeName() const;
};