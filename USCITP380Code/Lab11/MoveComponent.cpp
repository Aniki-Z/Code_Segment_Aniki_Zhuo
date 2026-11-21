#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
: Component(owner, 50)
, mAngularSpeed(0.0f)
, mForwardSpeed(0.0f)
, mStrafeSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	if (mAngularSpeed != 0.0f)
	{
		float mRotation = mOwner->GetRotation();
		mRotation += mAngularSpeed * deltaTime;
		mOwner->SetRotation(mRotation);
	}

	if (mForwardSpeed != 0.0f || mStrafeSpeed != 0.0f)
	{
		Vector3 mPosition = mOwner->GetPosition();
		Vector3 mForward = mOwner->GetForward();
		Vector3 mRight = mOwner->GetRight();

		mPosition += mForward * mForwardSpeed * deltaTime;
		mPosition += mRight * mStrafeSpeed * deltaTime;

		mOwner->SetPosition(mPosition);
	}
}
