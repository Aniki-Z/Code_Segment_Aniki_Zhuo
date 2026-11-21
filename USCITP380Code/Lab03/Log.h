#pragma once
#include "Game.h"

class Log : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class CollisionComponent* mCollisionComponent;
	class WrappingMove* mMove;

	int mRow;

public:
	Log(class Game* game, char mType, int row);
	~Log();

	static constexpr float LOG_HEIGHT = 24.0f;
	static constexpr float LOG_X_WIDTH = 96.0f;
	static constexpr float LOG_Y_WIDTH = 128.0f;
	static constexpr float LOG_Z_WIDTH = 192.0f;
	static constexpr float LOG_SPEED = 37.5f;

	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
	const WrappingMove* GetWrappingMove() const { return mMove; }
};