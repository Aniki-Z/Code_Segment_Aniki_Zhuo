#pragma once
#include "Game.h"
#include "WrappingMove.h"

class Vehicle : public Actor
{
private:
	class SpriteComponent* mSpriteComponent;
	class CollisionComponent* mCollisionComponent;

	int mRow;

	WrappingMove* mMove;

public:
	Vehicle(class Game* game, char mType, int row);
	~Vehicle();

	static constexpr float CAR_SIZE = 32.0f;
	static constexpr float TRACK_XSIZE = 64.0f;
	static constexpr float TRACK_YSIZE = 24.0f;
	static constexpr float MOVE_SPEED = 50.0f;

	void OnUpdate(float deltaTime) override;

	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
};