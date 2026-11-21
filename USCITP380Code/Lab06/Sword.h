#pragma once
#include "Game.h"

class Sword : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;

public:
	Sword(class Game* game);
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
	void SetCollisionSize(float width, float height);

	static constexpr float SWORD_SIZE = 28.0f;
};