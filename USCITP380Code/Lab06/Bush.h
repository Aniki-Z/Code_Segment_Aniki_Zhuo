#pragma once
#include "Game.h"

class Bush : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;
	class SpriteComponent* mSpriteComponent;
	class EnemyComponent* mEnemyComponent;

public:
	Bush(class Game* game);
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }

	static constexpr float BUSH_SIZE = 32.0f;
};