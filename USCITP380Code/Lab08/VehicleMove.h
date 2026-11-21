#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

enum class TurnState
{
	TurnLeft,
	TurnRight,
	NotTurn
};

class VehicleMove : public Component
{
private:
	bool mIsAccelerate;
	TurnState mTurnState;
	Vector3 mVelocity;
	float mAngularVelocity;
	float mAccelerateTimer;

	std::vector<std::vector<int>> mCheckpoints;
	int mCurrentLap;
	int mLastCheckpointIndex;
	int mCheckpoint;

public:
	VehicleMove(class Actor* owner);

	void Update(float deltaTime) override;

	void SetCarState(TurnState state) { mTurnState = state; };
	void SetAccelerate(bool state) { mIsAccelerate = state; };
	Vector3 GetVelocity() { return mVelocity; };
	void SetVelocity(Vector3 velocity) { mVelocity = velocity; };

	virtual void OnLapChange(int newLap) {};
	int GetLap() const { return mCurrentLap; }
	int GetChekPoint() const { return mCheckpoint; }
	std::vector<int> GetCheckPointVector(int index) const { return mCheckpoints[index]; }

	void ApplyAcceleration(float deltaTime);
	void UpdatePosition(float deltaTime);
	void ApplyDrag();
	void UpdateRotation(float deltaTime);
	void CheckCheckpointProgression();
	float GetDistanceToNextCheckpoint() const;

	const float MIN_ACC_MAG = 1000.0f;
	const float MAX_ACC_MAG = 2500.0f;
	const float ACC_RAMP_TIME = 2.0f;
	const float ANGULAR_ACC = 5.0f * Math::Pi;
	const float LINEAR_DRAG = 0.9f;
	const float LINEAR_DRAG_WITHOUT_ACC = 0.975f;
	const float ANGULAR_DRAG = 0.9f;
};