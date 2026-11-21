#pragma once
#include "MoveComponent.h"
#include <vector>
#include "Math.h"

class GoombaMove : public MoveComponent
{
private:
	float mDirection;
	float mVelocityY;

public:
	GoombaMove(class Actor* owner);

	void Update(float deltaTime) override;

	static constexpr float SPEED = 100.0f;
	static constexpr float GRAVITY = 2000.0f;
};