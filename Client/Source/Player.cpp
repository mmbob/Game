#include "Player.h"

#include <algorithm>

#include "Engine.h"

Player::Player(Renderer* pRenderer, Engine* pEngine)
	: RenderedEntity(pRenderer, pEngine), sanity(0), maxSpeed(8.0f), acceleration(0.0f, 0.0f)
{
	RECT clip = { 0, 0, -1, -1 };
	renderObject->SetTextureName(L"Player");
	renderObject->SetTextureClip(clip);

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_kinematicBody;

	b2PolygonShape box;
	box.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;

	b2Body* body = parent->GetPhysics()->CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
}

Player::~Player()
{

}

void Player::Update(DirectXManager* pDirectX)
{
	const float accelValue = 1.0f;
	bool left = pDirectX->IsKeyPressed(DIK_A);
	bool up = pDirectX->IsKeyPressed(DIK_W);
	bool right = pDirectX->IsKeyPressed(DIK_D);
	bool down = pDirectX->IsKeyPressed(DIK_S);

	b2Vec2 force = b2Vec2_zero;

	if (left && !right)
		force.x = -accelValue;
	else if (right && !left)
		force.x = accelValue;
	if (up && !down)
		force.y = -accelValue;
	else if (down && !up)
		force.y = accelValue;

	body->ApplyForceToCenter(force);

	velocity *= 0.9f;

	velocity.x = std::min<float>(std::max<float>(velocity.x + acceleration.x, -maxSpeed), maxSpeed);
	velocity.y = std::min<float>(std::max<float>(velocity.y + acceleration.y, -maxSpeed), maxSpeed);
}