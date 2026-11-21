#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "LaserComponent.h"
#include "AudioSystem.h"
#include <unordered_map>

enum class TurretState
{
	Idle,
	Search,
	Priming,
	Firing,
	Falling,
	Dead
};

class TurretHead : public Actor
{
public:
	TurretHead(Game* game, Actor* parent);
	void OnUpdate(float deltaTime) override;

	const float SCALE = 0.75f;

	void ChangeState(TurretState state);

	void UpdateIdle(float deltaTime);
	void UpdateSearch(float deltaTime);
	void UpdatePriming(float deltaTime);
	void UpdateFiring(float deltaTime);
	void UpdateFalling(float deltaTime);
	void UpdateDead(float deltaTime);

	void CheckForTarget();
	void SetupNewSearchTarget();
	bool CheckPortalTeleport(float deltaTime);
	void Die();
	void TakeDamage();

private:
	MeshComponent* mMeshComponent;

	Actor* mLaserActor;
	LaserComponent* mLaser;

	TurretState mTurretState;
	float mStateTimer;

	class Actor* mTarget;

	Quaternion mSearchStartRot;
	Quaternion mSearchEndRot;
	float mSearchPhaseTime;
	bool mSearchForward;

	Vector3 mFallVelocity;
	float mTeleportCooldown;

	float mFiringTime;
	std::unordered_map<TurretState, std::string> mStateSoundMap;
	SoundHandle mVOSoundHandle;

	bool mFriendlyFirePlayed;
};
