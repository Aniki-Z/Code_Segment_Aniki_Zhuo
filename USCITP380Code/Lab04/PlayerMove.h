#pragma once
#include "MoveComponent.h"

class PlayerMove : public MoveComponent
{
private:
	float mForwardSpeed;
	float mYSpeed;

	bool mSpacePressed;
	bool mInAir;

public:
	PlayerMove(class Actor* owner);

	void ProcessInput(const Uint8* keyState) override;
	void Update(float deltaTime) override;
	void UpdateAnimation();

	static constexpr float GRAVITY = 2000.0f;
	static constexpr float GROUND_LOCATION = 448.0f;
	static constexpr float MOVE_SPEED = 300.0f;
	static constexpr float JUMP_SPEED = -700.0f;
	static constexpr float HALF_SCREEN_LOCATION = 300.0f;
	static constexpr float AFTER_STOMP_SPEED = -350.0f;
	static constexpr float WIN_LOCATION = 6368.0f;
};
