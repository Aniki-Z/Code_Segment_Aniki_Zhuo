#pragma once
#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"

class WrappingMove : public MoveComponent
{
private:
	Vector2 mDirection;

public:
	WrappingMove(Actor* mOwner);

	void Update(float deltaTime) override;
	void SetDirection(const Vector2& dir) { mDirection = dir; }
	Vector2 GetDirection() const { return mDirection; }

	static constexpr float SCREEN_WIDTH = 448.0f;
};
