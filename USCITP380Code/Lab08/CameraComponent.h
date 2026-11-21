#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
private:
	Vector3 mCameraPosition;
	Vector3 mCameraVelocity;

public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime) override;
	Vector3 IdealPosition();
	void SnapToIdeal();

	const float H_DISTANCE = 60.0f;
	const float V_DISTANCE = 70.0f;
	const float TARGET_OFFSET = 50.0f;
	const float SPRING_CONSTANT = 256.0f;
	const float DAMP_CONSTANT = 2.0f * sqrt(SPRING_CONSTANT);
};