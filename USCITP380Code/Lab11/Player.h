#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "PlayerMesh.h"
#include "HealthComponent.h"

class Player : public Actor
{
private:
	class PlayerMove* mPlayerMove;
	class CameraComponent* mCameraComponent;
	class CollisionComponent* mCollisionComponent;
	bool mHasGun;

	Vector3 mInitialPos;

	class HealthComponent* mHealthComponent;

public:
	Player(Game* game);

	PlayerMove* GetPlayerMove() const { return mPlayerMove; }

	bool HasGun() const { return mHasGun; }

	CollisionComponent* GetCollisionComponent() { return mCollisionComponent; }
	CameraComponent* GetCameraComponent() { return mCameraComponent; }

	const Vector3& GetInitialPos() const { return mInitialPos; }
	void SetInitialPos(Vector3 vector) { mInitialPos = vector; }

	void HasGunNow();

	HealthComponent* GetHealthComponent() const { return mHealthComponent; }
};
