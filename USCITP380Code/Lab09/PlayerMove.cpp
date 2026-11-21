#include "PlayerMove.h"
#include "Random.h"
#include "Game.h"
#include "Block.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
, mForwardSpeed(0.0f)
, mStrafeSpeed(0.0f)
, mCurrentState(OnGround)
, mGravity(0.0f, 0.0f, -980.0f)
, mJumpForce(0.0f, 0.0f, 35000.0f)
, mMass(1.0f)
, mPrevLeftMouse(false)
, mPrevRightMouse(false)
, mKeyState(false)
{
	ChangeState(Falling);
	mCrosshair = new Crosshair(owner);
}

void PlayerMove::ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							  const Vector2& relativeMouse)
{

	if (keyState[SDL_SCANCODE_W])
	{
		AddForce(mOwner->GetForward() * SPEED);
	}
	if (keyState[SDL_SCANCODE_S])
	{
		AddForce(mOwner->GetForward() * -SPEED);
	}

	if (keyState[SDL_SCANCODE_D])
	{
		AddForce(mOwner->GetRight() * SPEED);
	}
	if (keyState[SDL_SCANCODE_A])
	{
		AddForce(mOwner->GetRight() * -SPEED);
	}

	if (keyState[SDL_SCANCODE_SPACE] && mCurrentState == OnGround && !mKeyState)
	{
		AddForce(mJumpForce);
		ChangeState(Jump);
	}

	if (keyState[SDL_SCANCODE_R])
	{
		if (GetGame()->GetBluePortal())
		{
			GetGame()->GetBluePortal()->SetState(ActorState::Destroy);
			GetGame()->SetBluePortal(nullptr);
		}

		if (GetGame()->GetOrangePortal())
		{
			GetGame()->GetOrangePortal()->SetState(ActorState::Destroy);
			GetGame()->SetOrangePortal(nullptr);
		}

		mCrosshair->SetState(CrosshairState::Default);
	}

	float angularSpeed = (relativeMouse.x / 500.0f) * Math::Pi * 10.0f;
	SetAngularSpeed(angularSpeed);

	float pitchSpeed = (relativeMouse.y / 500.0f) * Math::Pi * 10.0f;

	if (CameraComponent* camera = mOwner->GetComponent<CameraComponent>())
	{
		camera->SetPitchSpeed(pitchSpeed);
	}

	bool leftMousePressed = mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool rightMousePressed = mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT);

	if (mOwner->GetGame()->GetPlayer()->HasGun())
	{
		if (leftMousePressed && !mPrevLeftMouse)
		{
			CreatePortal(true);
		}
		else if (rightMousePressed && !mPrevRightMouse)
		{
			CreatePortal(false);
		}
	}

	mPrevLeftMouse = leftMousePressed;
	mPrevRightMouse = rightMousePressed;
	mKeyState = keyState[SDL_SCANCODE_SPACE];
}

void PlayerMove::Update(float deltaTime)
{
	switch (mCurrentState)
	{
	case OnGround:
		UpdateOnGround(deltaTime);
		break;
	case Jump:
		UpdateJump(deltaTime);
		break;
	case Falling:
		UpdateFalling(deltaTime);
		break;
	default:
		break;
	}
}

void PlayerMove::ChangeState(MoveState newState)
{
	mCurrentState = newState;
}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);

	CollisionComponent* self = mOwner->GetGame()->GetPlayer()->GetCollisionComponent();
	bool isOnGround = false;

	for (auto collider : mOwner->GetGame()->GetColliders())
	{
		CollisionComponent* other = collider->GetComponent<CollisionComponent>();
		CollSide side = FixCollision(self, other);
		if (side == CollSide::Top)
		{
			isOnGround = true;
		}
	}

	if (!isOnGround)
	{
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateJump(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	CollisionComponent* self = mOwner->GetGame()->GetPlayer()->GetCollisionComponent();
	for (auto collider : mOwner->GetGame()->GetColliders())
	{
		CollisionComponent* other = collider->GetComponent<CollisionComponent>();
		CollSide side = FixCollision(self, other);
		if (side == CollSide::Bottom)
		{
			mVelocity.z = 0.0f;
		}
	}

	if (mVelocity.z <= 0.0f)
	{
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateFalling(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	bool isOnGround = false;
	CollisionComponent* self = mOwner->GetGame()->GetPlayer()->GetCollisionComponent();
	for (auto collider : mOwner->GetGame()->GetColliders())
	{
		CollisionComponent* other = collider->GetComponent<CollisionComponent>();
		CollSide side = FixCollision(self, other);
		if (side == CollSide::Top && mVelocity.z <= 0.0f)
		{
			isOnGround = true;
		}
	}

	if (isOnGround)
	{
		mVelocity.z = 0.0f;
		ChangeState(OnGround);
	}
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;

	FixXYVelocity();

	mOwner->SetPosition(mOwner->GetPosition() + mVelocity * deltaTime);
	mOwner->SetRotation(mOwner->GetRotation() + GetAngularSpeed() * deltaTime);

	mPendingForces = Vector3::Zero;
}

void PlayerMove::AddForce(const Vector3& force)
{
	mPendingForces += force;
}

void PlayerMove::FixXYVelocity()
{
	Vector2 xyVelocity(mVelocity.x, mVelocity.y);

	if (xyVelocity.Length() >= MAX_SPEED)
	{
		xyVelocity.Normalize();
		xyVelocity *= MAX_SPEED;
	}

	if (mCurrentState == OnGround)
	{
		if (Math::NearlyZero(mAcceleration.x) || (mAcceleration.x * xyVelocity.x <= 0.0f))
		{
			xyVelocity.x *= BREAKING_FACTOR;
		}
		if (Math::NearlyZero(mAcceleration.y) || (mAcceleration.y * xyVelocity.y <= 0.0f))
		{
			xyVelocity.y *= BREAKING_FACTOR;
		}
	}

	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}

CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* collider)
{
	Vector3 offset;
	CollSide side = self->GetMinOverlap(collider, offset);

	if (side != CollSide::None)
	{
		mOwner->SetPosition(mOwner->GetPosition() + offset);
	}

	return side;
}

void PlayerMove::CreatePortal(bool isBlue)
{
	Game* game = mOwner->GetGame();

	Vector3 nearPlane = game->GetRenderer()->Unproject(Vector3(0.0f, 0.0f, 0.0f));
	Vector3 farPlane = game->GetRenderer()->Unproject(Vector3(0.0f, 0.0f, 1.0f));

	Vector3 direction = (farPlane - nearPlane);
	direction.Normalize();

	LineSegment lineSegment(nearPlane, nearPlane + direction * 1000.0f);

	CastInfo castInfo;
	if (!SegmentCast(game->GetColliders(), lineSegment, castInfo))
	{
		return;
	}

	Block* hitBlock = dynamic_cast<Block*>(castInfo.mActor);
	if (!hitBlock)
	{
		return;
	}

	Portal* existingPortal = nullptr;
	if (isBlue)
	{
		existingPortal = game->GetBluePortal();
	}
	else
	{
		existingPortal = game->GetOrangePortal();
	}

	if (existingPortal)
	{
		existingPortal->SetState(ActorState::Destroy);
	}

	Portal* newPortal = new Portal(game, isBlue);
	newPortal->SetPosition(castInfo.mPoint);

	Vector3 normal = castInfo.mNormal;
	Vector3 originalFacing = Vector3::UnitX;

	float dotProduct = Vector3::Dot(originalFacing, normal);

	Quaternion rotation;

	if (dotProduct >= 1.0f)
	{
		rotation = Quaternion::Identity;
	}
	else if (dotProduct <= -1.0f)
	{
		rotation = Quaternion(Vector3::UnitZ, Math::Pi);
	}
	else
	{
		Vector3 axis = Vector3::Cross(originalFacing, normal);
		axis.Normalize();

		float angle = acos(dotProduct);
		rotation = Quaternion(axis, angle);
	}

	newPortal->SetQuat(rotation);

	if (isBlue)
	{
		game->SetBluePortal(newPortal);
	}
	else
	{
		game->SetOrangePortal(newPortal);
	}

	if (game->GetBluePortal() != nullptr)
	{
		mCrosshair->SetState(CrosshairState::BlueFill);
	}
	else if (game->GetOrangePortal() != nullptr)
	{
		mCrosshair->SetState(CrosshairState::OrangeFill);
	}
	else if (game->GetBluePortal() != nullptr && game->GetOrangePortal() != nullptr)
	{
		mCrosshair->SetState(CrosshairState::BothFill);
	}
}