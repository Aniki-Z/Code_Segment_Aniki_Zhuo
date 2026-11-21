#pragma once
#include "Game.h"

class Soldier : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;
	class AnimatedSprite* mAnimatedSprite;
	class SoldierAI* mSoldierAI;
	class EnemyComponent* mEnemyComponent;

public:
	Soldier(class Game* game, PathNode* start, PathNode* end);
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
	AnimatedSprite* GetSoldierAnimatedSprite() { return mAnimatedSprite; };

	static constexpr float SOLDIER_SIZE = 32.0f;
};