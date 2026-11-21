#pragma once
#include "MoveComponent.h"
#include "Math.h"
#include "CollisionComponent.h"
#include "Crosshair.h"
#include "SegmentCast.h"

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

	bool mKeyState;

public:
	PlayerMove(class Actor* owner);

	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
					  const Vector2& relativeMouse) override;

	void CreatePortal(bool isBlue);

	const Vector3& GetVelocity() const { return mVelocity; }
	const Vector3& GetAcceleration() const { return mAcceleration; }

	const float SPEED = 700.0f;
	const float MAX_SPEED = 400.0f;
	const float BREAKING_FACTOR = 0.9f;
};
