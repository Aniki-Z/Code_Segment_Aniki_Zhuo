#include "PlayerMove.h"
#include "Random.h"
#include "Game.h"
#include "Block.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
, mForwardSpeed(0.0f)
, mStrafeSpeed(0.0f)
, mCurrentState(OnGround)
, mPrevState(mCurrentState)
, mGravity(0.0f, 0.0f, -980.0f)
, mJumpForce(0.0f, 0.0f, 35000.0f)
, mMass(1.0f)
, mPrevLeftMouse(false)
, mPrevRightMouse(false)
, mPortalCooldown(0.0f)
, mKeyState(false)
, mTeleport(false)
, mDeathSound(SoundHandle::Invalid)
, mFirstTime(false)
, mIndex(-1)
, mDead(false)
{
	mFootstepSound = GetGame()->GetAudio()->PlaySound("FootstepLoop.ogg", true);
	GetGame()->GetAudio()->PauseSound(mFootstepSound);

	ChangeState(Falling);
	mCrosshair = new Crosshair(owner);

	mDeathSoundFiles = {"Glados-PlayerDead1.ogg", "Glados-PlayerDead2.ogg",
						"Glados-PlayerDead3.ogg", "Glados-PlayerDead4.ogg"};
	mDeathSubtitles = {
		"Congratulations! The test is now over.",
		"Thank you for participating in this Aperture Science computer-aided enrichment activity.",
		"Goodbye.", "You're not a good person. You know that, right?"};
}

PlayerMove::~PlayerMove()
{
	GetGame()->GetAudio()->StopAllSounds();
}

void PlayerMove::ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							  const Vector2& relativeMouse)
{
	if (GetGame()->GetPlayer()->GetHealthComponent()->IsDead())
	{
		return;
	}

	mForwardSpeed = 0.0f;
	mStrafeSpeed = 0.0f;

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
		GetGame()->GetAudio()->PlaySound("Jump.ogg");
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
		GetGame()->GetAudio()->PlaySound("PortalClose.ogg");
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
	if (mDead)
	{
		if (!mFirstTime)
		{
			mIndex = Random::GetIntRange(0, mDeathSoundFiles.size() - 1);

			mDeathSound = GetGame()->GetAudio()->PlaySound(mDeathSoundFiles[mIndex]);

			GetGame()->GetPlayer()->GetHUD()->ShowSubtitle(mDeathSubtitles[mIndex]);

			mFirstTime = true;
		}

		if (GetGame()->GetAudio()->GetSoundState(mDeathSound) == SoundState::Stopped)
		{
			GetGame()->SetNextLevel(GetGame()->GetCurrentLevel());
		}
		return;
	}

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

	if (GetGame()->GetPlayer()->GetPosition().z <= -750.0f)
	{
		GetGame()->GetPlayer()->GetHealthComponent()->TakeDamage(
			Math::Infinity, GetGame()->GetPlayer()->GetPosition());
	}

	if (mCurrentState == OnGround && mVelocity.Length() > 50.0f)
	{
		GetGame()->GetAudio()->ResumeSound(mFootstepSound);
	}
	else
	{
		GetGame()->GetAudio()->PauseSound(mFootstepSound);
	}
}

void PlayerMove::ChangeState(MoveState newState)
{
	mPrevState = mCurrentState;
	mCurrentState = newState;
}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);

	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}

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

	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}

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

	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}

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
		mTeleport = false;
		GetGame()->GetAudio()->PlaySound("Land.ogg");
	}
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;

	FixXYVelocity();

	if (mVelocity.z <= -1000.0f)
	{
		mVelocity.z = -1000.0f;
	}

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
	if (!mTeleport)
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
		GetGame()->GetAudio()->PlaySound("PortalFail.ogg");
		return;
	}

	Block* hitBlock = dynamic_cast<Block*>(castInfo.mActor);
	if (!hitBlock)
	{
		GetGame()->GetAudio()->PlaySound("PortalFail.ogg");
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
	Vector3 axis;

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
		axis = Vector3::Cross(originalFacing, normal);
		axis.Normalize();

		float angle = acos(dotProduct);
		rotation = Quaternion(axis, angle);
	}

	newPortal->SetQuat(rotation);

	Vector3 normals = newPortal->GetQuatForward();
	normals.Normalize();
	if (!Math::NearlyZero(normals.x))
	{
		newPortal->GetCollisionComponent()->SetSize(110.0f, 125.0f, 10.0f);
	}
	else if (!Math::NearlyZero(normals.y))
	{
		newPortal->GetCollisionComponent()->SetSize(10.0f, 125.0f, 110.0f);
	}
	else
	{
		newPortal->GetCollisionComponent()->SetSize(110.0f, 10.0f, 125.0f);
	}

	if (isBlue)
	{
		game->SetBluePortal(newPortal);
		GetGame()->GetAudio()->PlaySound("PortalShootBlue.ogg");
	}
	else
	{
		game->SetOrangePortal(newPortal);
		GetGame()->GetAudio()->PlaySound("PortalShootOrange.ogg");
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

bool PlayerMove::UpdatePortalTeleport(float deltaTime)
{
	mPortalCooldown -= deltaTime;
	if (mPortalCooldown > 0.0f)
	{
		return false;
	}

	Game* game = mOwner->GetGame();
	Portal* blue = game->GetBluePortal();
	Portal* orange = game->GetOrangePortal();
	if (!blue || !orange)
	{
		return false;
	}

	CollisionComponent* playerColl = game->GetPlayer()->GetCollisionComponent();
	CollisionComponent* blueColl = blue->GetCollisionComponent();
	CollisionComponent* orangeColl = orange->GetCollisionComponent();

	if (playerColl->Intersect(blueColl))
	{
		CalculateTeleport(blue, orange);
		mPortalCooldown = 0.2f;
		ChangeState(Falling);
		mTeleport = true;
		return true;
	}
	else if (playerColl->Intersect(orangeColl))
	{
		CalculateTeleport(orange, blue);
		mPortalCooldown = 0.2f;
		ChangeState(Falling);
		mTeleport = true;
		return true;
	}

	return false;
}

void PlayerMove::CalculateTeleport(Portal* entry, Portal* exit)
{
	GetGame()->GetAudio()->PlaySound("PortalTeleport.ogg");
	Vector3 newFace;
	Vector3 newVelocityDir;
	Vector3 newPos;

	Player* player = mOwner->GetGame()->GetPlayer();
	Vector3 playerPositon = player->GetPosition();
	Vector3 playerForward = player->GetForward();
	Vector3 playerVelocityDir = mVelocity;
	playerVelocityDir.Normalize();

	Vector3 entryFace = entry->GetQuatForward();
	Vector3 exitFace = exit->GetQuatForward();
	entryFace.Normalize();
	exitFace.Normalize();

	bool entryOnZ = false;
	bool exitOnZ = false;

	Vector3 entryOnZs = Vector3::Cross(Vector3::UnitZ, entryFace);
	Vector3 exitOnZs = Vector3::Cross(Vector3::UnitZ, exitFace);

	if (Math::NearlyZero(entryOnZs.x) && Math::NearlyZero(entryOnZs.y) &&
		Math::NearlyZero(entryOnZs.z))
	{
		entryOnZ = true;
	}

	if (Math::NearlyZero(exitOnZs.x) && Math::NearlyZero(exitOnZs.y) &&
		Math::NearlyZero(exitOnZs.z))
	{
		exitOnZ = true;
	}

	if (!exitOnZ)
	{
		if (entryOnZ)
		{
			newFace = exitFace;
		}
		else
		{
			newFace = entry->GetPortalOutVector(playerForward, exit, 0.0f);
		}
		newFace.Normalize();

		float angle = acos(Vector3::Dot(Vector3::UnitX, newFace));

		Vector3 axis = Vector3::Cross(Vector3::UnitX, newFace);
		axis.Normalize();

		if (axis.z < 0.0f)
		{
			angle = -angle;
		}

		player->SetRotation(angle);
	}

	float speed = Math::Max(1.5f * mVelocity.Length(), 350.0f);

	if (exitOnZ || entryOnZ)
	{
		newVelocityDir = exitFace;
	}
	else
	{
		newVelocityDir = entry->GetPortalOutVector(playerVelocityDir, exit, 0.0f);
	}

	newVelocityDir.Normalize();
	mVelocity = newVelocityDir * speed;

	if (exitOnZ || entryOnZ)
	{
		newPos = exit->GetPosition();
	}
	else
	{
		newPos = entry->GetPortalOutVector(playerPositon, exit, 1.0f);
	}

	newPos += exit->GetQuatForward() * 50.0f;

	player->SetPosition(newPos);
}