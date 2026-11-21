#pragma once
#include "Game.h"

class Block : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;
	class MeshComponent* mMeshComponent;

public:
	Block(Game* game);
	~Block();

	const float SCALE = 64.0f;
	const float SIZE = 1.0f;
};
