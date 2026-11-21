#include "TurretHead.h"
#include "Random.h"
#include "TurretBase.h"

TurretHead::TurretHead(Game* game, Actor* parent)
: Actor(game, parent)
, mTurretState(TurretState::Idle)
, mStateTimer(0.0f)
, mTarget(nullptr)
, mSearchStartRot(Quaternion::Identity)
, mSearchEndRot(Quaternion::Identity)
, mSearchPhaseTime(0.0f)
, mSearchForward(0.0f)
, mFallVelocity(Vector3::Zero)
, mTeleportCooldown(0.0f)
, mFiringTime(0.0f)
, mVOSoundHandle(SoundHandle::Invalid)
, mFriendlyFirePlayed(false)
{
	SetScale(SCALE);
	SetPosition(Vector3(0.0f, 0.0f, 18.75f));

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretHead.gpmesh"));

	mLaserActor = new Actor(game, this);
	mLaserActor->SetPosition(Vector3(0.0f, 0.0f, 10.0f));

	mLaser = new LaserComponent(mLaserActor);
	mLaser->SetIgnoreActor(parent);

	mStateSoundMap = {
		{TurretState::Idle, "TurretIdle.ogg"},		 {TurretState::Search, "TurretSearch.ogg"},
		{TurretState::Priming, "TurretPriming.ogg"}, {TurretState::Firing, "TurretFiring.ogg"},
		{TurretState::Falling, "TurretFalling.ogg"}, {TurretState::Dead, "TurretDead.ogg"},
	};
}

void TurretHead::OnUpdate(float deltaTime)
{
	if (GetGame()->GetPlayer()->GetPlayerMove()->GetDead())
	{
		SetState(ActorState::Paused);
		return;
	}

	CheckForTarget();
	mStateTimer += deltaTime;

	switch (mTurretState)
	{
	case TurretState::Idle:
		UpdateIdle(deltaTime);
		break;
	case TurretState::Search:
		UpdateSearch(deltaTime);
		break;
	case TurretState::Priming:
		UpdatePriming(deltaTime);
		break;
	case TurretState::Firing:
		UpdateFiring(deltaTime);
		break;
	case TurretState::Falling:
		UpdateFalling(deltaTime);
		break;
	case TurretState::Dead:
		UpdateDead(deltaTime);
		break;
	default:
		break;
	}
}

void TurretHead::ChangeState(TurretState state)
{
	if (state == mTurretState)
	{
		return;
	}

	if (mVOSoundHandle != SoundHandle::Invalid)
	{
		GetGame()->GetAudio()->StopSound(mVOSoundHandle);
		mVOSoundHandle = SoundHandle::Invalid;
	}

	auto it = mStateSoundMap.find(state);
	if (it != mStateSoundMap.end())
	{
		mVOSoundHandle = GetGame()->GetAudio()->PlaySound(it->second, false, this);
	}

	if (state == TurretState::Search && mTurretState != TurretState::Search)
	{
		if (!mSearchForward && mSearchPhaseTime >= 0.5f)
		{
			SetupNewSearchTarget();
		}
	}

	mTurretState = state;
	mStateTimer = 0.0f;
}

void TurretHead::UpdateIdle(float deltaTime)
{
	if (CheckPortalTeleport(deltaTime))
	{
		return;
	}

	if (mTarget)
	{
		ChangeState(TurretState::Priming);
		return;
	}
}

void TurretHead::UpdateSearch(float deltaTime)
{
	if (CheckPortalTeleport(deltaTime))
	{
		return;
	}

	if (mStateTimer >= 5.0f)
	{
		ChangeState(TurretState::Idle);
		return;
	}

	if (mTarget)
	{
		ChangeState(TurretState::Priming);
		return;
	}

	float phaseDuration = 0.5f;
	mSearchPhaseTime += deltaTime;

	float t = Math::Clamp(mSearchPhaseTime / phaseDuration, 0.0f, 1.0f);

	SetQuat(Quaternion::Slerp(mSearchStartRot, mSearchEndRot, t));

	if (mSearchPhaseTime >= phaseDuration)
	{
		mSearchPhaseTime = 0.0f;

		if (mSearchForward)
		{
			mSearchStartRot = mSearchEndRot;
			mSearchEndRot = Quaternion::Identity;
			mSearchForward = false;
		}
		else
		{
			SetupNewSearchTarget();
			mSearchForward = true;
		}
	}
}

void TurretHead::UpdatePriming(float deltaTime)
{
	if (CheckPortalTeleport(deltaTime))
	{
		return;
	}

	if (!mTarget)
	{
		ChangeState(TurretState::Search);
		return;
	}
	else if (mStateTimer >= 1.5f)
	{
		ChangeState(TurretState::Firing);
		return;
	}
}

void TurretHead::UpdateFiring(float deltaTime)
{
	mFiringTime += deltaTime;

	if (CheckPortalTeleport(deltaTime))
	{
		mFiringTime = 0.0f;
		return;
	}

	if (!mTarget)
	{
		ChangeState(TurretState::Search);
		mFiringTime = 0.0f;
		return;
	}

	if (mFiringTime >= 0.05f)
	{
		mFiringTime = 0.0f;
		GetGame()->GetAudio()->PlaySound("Bullet.ogg", false, mTarget);
		mTarget->GetComponent<HealthComponent>()->TakeDamage(2.5f, GetWorldPosition());
	}
}

void TurretHead::UpdateFalling(float deltaTime)
{
	Actor* base = GetParent();
	if (!base)
	{
		return;
	}

	Vector3 newPos = base->GetPosition() + mFallVelocity * deltaTime;
	base->SetPosition(newPos);

	if (CheckPortalTeleport(deltaTime))
	{
		return;
	}

	Vector3 gravity(0.0f, 0.0f, -980.0f);
	mFallVelocity += gravity * deltaTime;

	if (mFallVelocity.Length() > 800.0f)
	{
		mFallVelocity.Normalize();
		mFallVelocity *= 800.0f;
	}

	CollisionComponent* cc = base->GetComponent<CollisionComponent>();
	if (!cc)
	{
		return;
	}

	std::vector<Actor*> colliders = GetGame()->GetColliders();
	for (Actor* collider : colliders)
	{
		CollisionComponent* other = collider->GetComponent<CollisionComponent>();
		if (!other || other == cc)
		{
			continue;
		}

		Vector3 offset;
		CollSide side = cc->GetMinOverlap(other, offset);
		Vector3 basePos = base->GetPosition();
		base->SetPosition(basePos + offset);

		if (side == CollSide::Top && mFallVelocity.z <= 0.0f)
		{
			Vector3 basePos = base->GetPosition();
			basePos.z -= 15.0f;
			base->SetPosition(basePos);

			TurretBase* otherBase = dynamic_cast<TurretBase*>(collider);
			if (otherBase)
			{
				CollisionComponent* otherCC = otherBase->GetComponent<CollisionComponent>();
				if (otherCC)
				{
					basePos.z -= otherCC->GetHeight() / 2.0f;
					base->SetPosition(basePos);

					otherBase->Die();
				}
			}

			Die();
		}

		if (side == CollSide::Top)
		{
			mFallVelocity.z = 0.0f;
		}
	}
}

void TurretHead::UpdateDead(float deltaTime)
{
	if (CheckPortalTeleport(deltaTime))
	{
		return;
	}
}

void TurretHead::CheckForTarget()
{
	Actor* hit = mLaser->GetLastHitActor();
	if (hit && hit->GetComponent<HealthComponent>() &&
		!hit->GetComponent<HealthComponent>()->IsDead())
	{
		mTarget = hit;
	}
	else
	{
		mTarget = nullptr;
	}
}

void TurretHead::SetupNewSearchTarget()
{
	float angle = Random::GetFloatRange(0.0f, Math::TwoPi);

	Vector3 forward = Vector3::UnitX * 200.0f;
	Vector3 side = Vector3::UnitY * 75.0f * Math::Cos(angle);
	Vector3 up = Vector3::UnitZ * 25.0f * Math::Sin(angle);

	Vector3 target = GetWorldPosition() + forward + side + up;
	Vector3 from = Vector3::UnitX;
	Vector3 to = Vector3::Normalize(target - GetWorldPosition());

	float dot = Vector3::Dot(from, to);
	if (dot > 1.0f)
	{
		mSearchEndRot = Quaternion::Identity;
	}
	else if (dot < -1.0f)
	{
		Vector3 axis = Vector3::Cross(Vector3::UnitY, from);
		if (axis.LengthSq() < 0.01f)
		{
			axis = Vector3::Cross(Vector3::UnitZ, from);
		}
		axis.Normalize();
		mSearchEndRot = Quaternion(axis, Math::Pi);
	}
	else
	{
		Vector3 axis = Vector3::Cross(from, to);
		axis.Normalize();
		float angleBetween = Math::Acos(dot);
		mSearchEndRot = Quaternion(axis, angleBetween);
	}

	mSearchStartRot = Quaternion::Identity;
	mSearchForward = true;
	mSearchPhaseTime = 0.0f;
}

bool TurretHead::CheckPortalTeleport(float deltaTime)
{
	mTeleportCooldown -= deltaTime;
	if (mTeleportCooldown > 0.0f)
	{
		return false;
	}

	Portal* blue = GetGame()->GetBluePortal();
	Portal* orange = GetGame()->GetOrangePortal();
	if (!blue || !orange)
	{
		return false;
	}

	Actor* base = GetParent();
	CollisionComponent* cc = nullptr;
	if (base)
	{
		cc = base->GetComponent<CollisionComponent>();
	}
	if (!cc)
	{
		return false;
	}

	Portal* enter = nullptr;
	Portal* exit = nullptr;
	if (cc->Intersect(blue->GetCollisionComponent()))
	{
		enter = blue;
		exit = orange;
	}
	else if (cc->Intersect(orange->GetCollisionComponent()))
	{
		enter = orange;
		exit = blue;
	}

	if (!enter || !exit)
	{
		return false;
	}

	base->SetPosition(exit->GetPosition());

	Vector3 launchDir = exit->GetQuatForward();
	mFallVelocity += launchDir * 250.0f;

	mTeleportCooldown = 0.25f;
	ChangeState(TurretState::Falling);
	return true;
}

void TurretHead::Die()
{
	ChangeState(TurretState::Dead);

	Actor* base = GetParent();
	if (base)
	{
		Quaternion q = Quaternion(Vector3::UnitX, Math::PiOver2);
		base->SetQuat(q);
	}

	if (mLaser)
	{
		mLaser->SetIsAble(false);
	}
}

void TurretHead::TakeDamage()
{
	if (mFriendlyFirePlayed)
	{
		return;
	}

	if (mVOSoundHandle.IsValid())
	{
		GetGame()->GetAudio()->StopSound(mVOSoundHandle);
		mVOSoundHandle = SoundHandle::Invalid;
	}

	mVOSoundHandle = GetGame()->GetAudio()->PlaySound("TurretFriendlyFire.ogg", false, this);

	mFriendlyFirePlayed = true;
}
