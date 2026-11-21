#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
private:
	float mPitchAngle;
	float mPitchSpeed;

	Vector3 mCameraForward;

public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime) override;

	float GetPitchSpeed() const { return mPitchSpeed; }
	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }

	float GetPitchAngle() const { return mPitchAngle; }
	void SetPitchAngle(float angle) { mPitchAngle = angle; }
	Vector3 GetCameraForward() const { return mCameraForward; }

	const float TARGET_OFFSET = 50.0f;
};