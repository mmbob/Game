#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <memory>

#include "Renderer.h"
#include "RenderObject.h"

using namespace std;

class Entity
{
protected:
	D3DXVECTOR3 position;
	D3DXMATRIX rotation;
	D3DXVECTOR2 velocity;
public:
	virtual ~Entity();
};

class RenderedEntity : Entity
{
protected:
	RenderObject* pRenderObject;
public:
	RenderedEntity(Renderer* pRenderer);
	virtual ~RenderedEntity();
};