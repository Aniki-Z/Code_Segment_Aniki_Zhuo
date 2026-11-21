#pragma once
#include "Game.h"

class Block : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class CollisionComponent* mCollisionComponent;

public:
	Block(class Game* game, char mType);
	~Block();
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }

	static constexpr float BLOCK_SIZE = 32.0f;
};