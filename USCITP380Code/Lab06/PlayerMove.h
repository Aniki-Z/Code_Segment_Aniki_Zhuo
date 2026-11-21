#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "Sword.h"

enum class Direction
{
	None,
	Up,
	Down,
	Left,
	Right
};

class PlayerMove : public MoveComponent
{
private:
	Direction mDirection;
	Direction mLastDirection;
	Vector2 mVelocity;

	bool mIsAttacking;
	float mAttackTimer;

	class Sword* mSword;

	Uint8 mPrevKeyState[SDL_NUM_SCANCODES];

public:
	PlayerMove(class Actor* owner);

	void ProcessInput(const Uint8* keyState) override;
	void Update(float deltaTime) override;
	void UpdateAnimation();

	void UpdateSwordPosition();

	static constexpr float SPEED = 150.0f;
};
