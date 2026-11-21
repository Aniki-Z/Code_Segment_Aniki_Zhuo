#pragma once
#include "MoveComponent.h"
#include "Math.h"
#include "Game.h"
#include "Bullet.h"

class PlayerMove : public MoveComponent
{
private:
	Vector3 mVelocity;
	float mNextBlock;
	int mLastBlockIndex;
	class Bullet* mBullet;
	Uint8 mPrevKeyState[SDL_NUM_SCANCODES];
	float mInvulnerableTime;
	float mSpeedMultiplier;
	float mElapsedTime;
	bool mIsRolling;
	float mRollTimer;
	SoundHandle mAlarm;

public:
	PlayerMove(class Actor* owner);

	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;

	float GetSpeedMultiplier() const { return mSpeedMultiplier; };

	void UpdateShieldAndAudio(float deltaTime);
	void UpdateSpeed(float deltaTime);
	void UpdatePosition(float deltaTime);
	void HandleCollisions();
	void UpdateCameraView();
	void SpawnSideBlocks();
	void LoadObstacleBlocks();
	void UpdateRolling(float deltaTime);
	void AddingShield();

	const float MOVE_SPEED = 300.0f;
	const float Y_MAX = 180.0f;
	const float X_MAX = 225.0f;
	const float H_DISTANCE = 300.0f;
	const float TARGET_DISTANCE = 20.0f;
	const float SIDE_BLOCKS_FIX_POSITION = 500.0f;
	const float CHECK_DISTANCE = 4000.0f;
	const float ROLL_LINEAR_SPEED = 8.0f;
	const float MAX_ELPASED_TIME = 10.0f;
	const float SPEED_MULTIPLIER = 0.15f;
};
