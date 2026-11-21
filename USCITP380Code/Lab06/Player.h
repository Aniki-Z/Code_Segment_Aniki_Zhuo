#pragma once
#include "Game.h"
#include "PlayerMove.h"

class Player : public Actor
{
private:
	class AnimatedSprite* mAnimatedSprite;
	class CollisionComponent* mCollisionComponent;
	class PlayerMove* mPlayerMove;

public:
	Player(class Game* game);
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
	AnimatedSprite* GetAnimatedSprite() { return mAnimatedSprite; };

	static constexpr float PLAYER_SIZE = 20.0f;
};