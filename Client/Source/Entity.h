#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

using namespace std;

class Entity
{
public:
	D3DXVECTOR3 position;
	D3DXMATRIX rotation;

	D3DXVECTOR2 velocity;

	wstring textureName;
public:
	virtual ~Entity() = 0;


};