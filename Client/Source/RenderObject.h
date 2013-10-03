#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

class Renderer;

using namespace std;

class RenderObject
{
protected:
	Renderer* parent;

	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;

	wstring textureName;
	RECT textureClip;
public:
	RenderObject();
	virtual ~RenderObject();



	friend class Renderer;
};