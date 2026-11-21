#include "CameraComponent.h"
#include "Game.h"

CameraComponent::CameraComponent(Actor* owner)
: Component(owner)
, mPitchAngle(0.0f)
, mPitchSpeed(0.0f)
, mCameraForward(Vector3::UnitX)
{
}

void CameraComponent::Update(float deltaTime)
{
	mPitchAngle += mPitchSpeed * deltaTime;

	mPitchAngle = Math::Clamp(mPitchAngle, -Math::Pi / 2.1f, Math::Pi / 2.1f);

	Matrix4 pitchMatrix = Matrix4::CreateRotationY(mPitchAngle);

	Matrix4 yawMatrix = Matrix4::CreateRotationZ(mOwner->GetRotation());

	Matrix4 combinedMatrix = pitchMatrix * yawMatrix;

	mCameraForward = Vector3::Transform(Vector3::UnitX, combinedMatrix);

	Vector3 eye = mOwner->GetPosition();
	Vector3 target = eye + (mCameraForward * TARGET_OFFSET);

	Matrix4 view = Matrix4::CreateLookAt(eye, target, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(view);
}