#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game, Actor* parent)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector3::Zero)
, mScale(1.0f)
, mRotation(0.0f)
, mRollAngle(0.0f)
, mParent(parent)
{
	if (mParent)
	{
		mParent->AddChild(this);
	}
	else
	{
		mGame->AddActor(this);
	}
}

Actor::~Actor()
{
	GetGame()->GetAudio()->RemoveActor(this);
	while (!mChildren.empty())
	{
		delete mChildren.back();
	}

	if (mParent)
	{
		mParent->RemoveChild(this);
	}
	else
	{
		mGame->RemoveActor(this);
	}

	for (size_t i = 0; i < mComponents.size(); i++)
	{
		delete mComponents[i];
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	CalcWorldTransform();

	if (mState == ActorState::Active)
	{
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}

	CalcWorldTransform();

	for (Actor* child : mChildren)
	{
		child->Update(deltaTime);
	}
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
	Matrix4 quatRot = Matrix4::CreateFromQuaternion(mQuat);
	Matrix4 transMatrix = Matrix4::CreateTranslation(mPosition);

	mWorldTransform = scaleMatrix * rotMatrixZ * rotMatrixX * quatRot * transMatrix;

	if (mParent)
	{
		if (mInheritScale)
		{
			mWorldTransform *= mParent->mWorldTransform;
		}
		else
		{
			mWorldTransform *= mParent->GetWorldRotTrans();
		}
	}
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

Matrix4 Actor::GetWorldRotTrans() const
{
	Matrix4 rotMatrixZ = Matrix4::CreateRotationZ(mRotation);
	Matrix4 rotMatrixX = Matrix4::CreateRotationX(mRollAngle);
	Matrix4 transMatrix = Matrix4::CreateTranslation(mPosition);
	Matrix4 quatRot = Matrix4::CreateFromQuaternion(mQuat);

	Matrix4 localTrans = rotMatrixZ * rotMatrixX * quatRot * transMatrix;

	if (mParent)
	{
		return localTrans * mParent->GetWorldRotTrans();
	}
	else
	{
		return localTrans;
	}
}

void Actor::AddChild(Actor* actor)
{
	mChildren.emplace_back(actor);
}

void Actor::RemoveChild(Actor* actor)
{
	auto iter = std::find(mChildren.begin(), mChildren.end(), actor);
	if (iter != mChildren.end())
	{
		mChildren.erase(iter);
	}
}