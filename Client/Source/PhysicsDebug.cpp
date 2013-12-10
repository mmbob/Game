#include "PhysicsDebug.h"

#include "Renderer.h"

PhysicsDebugDraw::PhysicsDebugDraw(Renderer* pRenderer)
: renderer(pRenderer)
{
	D3DXCreateLine(renderer->GetDirectX()->GetDevice(), &line);

	SetFlags(e_shapeBit);
}

PhysicsDebugDraw::~PhysicsDebugDraw()
{
	line->Release();
}

void PhysicsDebugDraw::DrawPolygon(const b2Vec2* rawVertices, int32 vertexCount, const b2Color& color)
{
	D3DXVECTOR2 vertices[50];
	for (int i = 0; i < vertexCount; ++i)
	{
		renderer->WorldToScreen(D3DXVECTOR3(rawVertices[i].x, rawVertices[i].y, 1.0f), &vertices[i]);
	}
	line->Begin();
	line->Draw(vertices, vertexCount + 1, D3DCOLOR_COLORVALUE(color.r, color.g, color.b, 1.0f));
	line->End();
}

void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* rawVertices, int32 vertexCount, const b2Color& color)
{
	bool onScreen = false;

	D3DXVECTOR2 vertices[50];
	for (int i = 0; i < vertexCount; ++i)
	{
		onScreen |= renderer->WorldToScreen(D3DXVECTOR3(rawVertices[i].x, rawVertices[i].y, 1.0f), &vertices[i]);
	}
	if (onScreen)
	{
		vertices[vertexCount] = vertices[0];
		line->Begin();
		line->Draw(vertices, vertexCount + 1, D3DCOLOR_COLORVALUE(color.r, color.g, color.b, 1.0f));
		line->End();
	}
}

void PhysicsDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{

}

void PhysicsDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{

}

void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{

}

void PhysicsDebugDraw::DrawTransform(const b2Transform& xf)
{

}