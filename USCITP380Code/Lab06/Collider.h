#pragma once
#include "Game.h"

class Collider : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;

public:
	Collider(class Game* game, int width, int height);
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
};