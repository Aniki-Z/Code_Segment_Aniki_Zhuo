#include "CameraComponent.h"
#include "Game.h"

CameraComponent::CameraComponent(Actor* owner)
: Component(owner)
, mCameraVelocity(Vector3::Zero)
{
	SnapToIdeal();
}

void CameraComponent::Update(float deltaTime)
{
	Vector3 idealPosition = IdealPosition();

	Vector3 displacement = mCameraPosition - idealPosition;
	Vector3 springAccel = (-SPRING_CONSTANT * displacement) - (DAMP_CONSTANT * mCameraVelocity);

	mCameraVelocity += springAccel * deltaTime;
	mCameraPosition += mCameraVelocity * deltaTime;

	Vector3 target = mOwner->GetPosition() + (mOwner->GetForward() * TARGET_OFFSET);

	Matrix4 view = Matrix4::CreateLookAt(mCameraPosition, target, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(view);
}

Vector3 CameraComponent::IdealPosition()
{
	Vector3 position = mOwner->GetPosition();
	Vector3 forward = mOwner->GetForward();

	Vector3 idealPosition = position - (forward * H_DISTANCE);
	idealPosition.z = V_DISTANCE;

	return idealPosition;
}

void CameraComponent::SnapToIdeal()
{
	mCameraPosition = IdealPosition();

	Vector3 target = mOwner->GetPosition() + (mOwner->GetForward() * TARGET_OFFSET);

	Matrix4 view = Matrix4::CreateLookAt(mCameraPosition, target, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(view);
}