#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector3::Zero)
, mScale(1.0f)
, mRotation(0.0f)
, mRollAngle(0.0f)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);

	for (size_t i = 0; i < mComponents.size(); i++)
	{
		delete mComponents[i];
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	if (mState == ActorState::Active)
	{
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}

	CalcWorldTransform();
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse)
{
	if (mState == ActorState::Active)
	{
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->ProcessInput(keyState, mouseButtons, relativeMouse);
		}
		OnProcessInput(keyState, mouseButtons, relativeMouse);
	}
}

void Actor::OnProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector3 Actor::GetForward() const
{
	float radians = GetRotation();
	return Vector3(Math::Cos(radians), Math::Sin(radians), 0.0f);
}

void Actor::CalcWorldTransform()
{
	Matrix4 scaleMatrix = Matrix4::CreateScale(mScale);
	Matrix4 rotMatrixZ = Matrix4::CreateRotationZ(mRotation);
	Matrix4 rotMatrixX = Matrix4::CreateRotationX(mRollAngle);
	Matrix4 transMatrix = Matrix4::CreateTranslation(mPosition);

	mWorldTransform = scaleMatrix * rotMatrixZ * rotMatrixX * Matrix4::CreateFromQuaternion(mQuat) *
					  transMatrix;
}

Vector3 Actor::GetQuatForward() const
{
	Vector3 fwd = Vector3::Transform(Vector3::UnitX, mQuat);
	fwd.Normalize();

	return fwd;
}

Vector3 Actor::GetRight() const
{
	float radians = GetRotation() + Math::PiOver2;
	return Vector3(Math::Cos(radians), Math::Sin(radians), 0.0f);
}
