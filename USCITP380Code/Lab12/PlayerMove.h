#pragma once
#include "MoveComponent.h"
#include "Math.h"
#include "CollisionComponent.h"
#include "Crosshair.h"
#include "SegmentCast.h"
#include "AudioSystem.h"

enum MoveState
{
	OnGround,
	Jump,
	Falling
};

class PlayerMove : public MoveComponent
{
private:
	float mForwardSpeed;
	float mStrafeSpeed;

	MoveState mCurrentState;
	MoveState mPrevState;

	void ChangeState(MoveState newState);

	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);

	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	Vector3 mGravity;
	Vector3 mJumpForce;

	float mMass;

	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);

	void FixXYVelocity();

	CollSide FixCollision(CollisionComponent* self, CollisionComponent* collider);

	class Crosshair* mCrosshair;
	bool mPrevLeftMouse;
	bool mPrevRightMouse;

	float mPortalCooldown;

	bool mKeyState;

	bool mTeleport;

	SoundHandle mFootstepSound;

	SoundHandle mDeathSound;
	std::vector<std::string> mDeathSoundFiles;
	std::vector<std::string> mDeathSubtitles;

	bool mFirstTime;
	int mIndex;

	bool mDead;

public:
	PlayerMove(class Actor* owner);
	~PlayerMove();

	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
					  const Vector2& relativeMouse) override;

	void CreatePortal(bool isBlue);

	bool UpdatePortalTeleport(float deltaTime);
	void CalculateTeleport(class Portal* entry, class Portal* exit);

	const Vector3& GetVelocity() const { return mVelocity; }
	const Vector3& GetAcceleration() const { return mAcceleration; }

	void SetDead(bool condition) { mDead = condition; }
	bool GetDead() const { return mDead; }

	const float SPEED = 700.0f;
	const float MAX_SPEED = 400.0f;
	const float BREAKING_FACTOR = 0.9f;
};
