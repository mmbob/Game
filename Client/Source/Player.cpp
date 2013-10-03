#include "Player.h"

Player::Player(Renderer* pRenderer)
	: RenderedEntity(pRenderer)
{
	position = D3DXVECTOR3(400, 400, 1.0f);
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixAffineTransformation2D(&rotation, 1.0f, nullptr, 0.0f, &D3DXVECTOR2(375, 225));
	velocity = D3DXVECTOR2(0.0f, 0.0f);

	textureName = L"Player";
}

Player::~Player()
{

}

void Player::Update(DirectXManager* pDirectX)
{
	bool left = pDirectX->IsKeyPressed(DIK_A);
	bool up = pDirectX->IsKeyPressed(DIK_W);
	bool right = pDirectX->IsKeyPressed(DIK_D);
	bool down = pDirectX->IsKeyPressed(DIK_S);

	if (left && !right)
		position.x -= 4;
	else if (up && !down)
		position.y -= 4;
	else if (right && !left)
		position.x += 4;
	else if (down && !up)
		position.y += 4;
}