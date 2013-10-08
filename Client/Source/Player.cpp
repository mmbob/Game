#include "Player.h"

#include <algorithm>

Player::Player(Renderer* pRenderer)
	: RenderedEntity(pRenderer), sanity(0), maxSpeed(8.0f), acceleration(0.0f, 0.0f)
{
	RECT clip = { 0, 0, -1, -1 };
	pRenderObject->SetTextureName(L"Player");
	pRenderObject->SetTextureClip(clip);
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

	acceleration = D3DXVECTOR2(0.0f, 0.0f);

	if (left && !right)
		acceleration.x = -accelValue;
	else if (right && !left)
		acceleration.x = accelValue;
	if (up && !down)
		acceleration.y = -accelValue;
	else if (down && !up)
		acceleration.y = accelValue;

	velocity *= 0.9f;

	velocity.x = std::min<float>(std::max<float>(velocity.x + acceleration.x, -maxSpeed), maxSpeed);
	velocity.y = std::min<float>(std::max<float>(velocity.y + acceleration.y, -maxSpeed), maxSpeed);
}