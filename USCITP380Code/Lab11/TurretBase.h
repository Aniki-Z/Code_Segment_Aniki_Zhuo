#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "TurretHead.h"
#include "HealthComponent.h"

class TurretBase : public Actor
{
private:
	MeshComponent* mMeshComponent;
	CollisionComponent* mCollisionComponent;

	TurretHead* mHead;

	HealthComponent* mHealthComponent;

public:
	TurretBase(Game* game);
	~TurretBase();

	void Die();

	const float SCALE = 0.75f;
	const float SIZE_X = 25.0f;
	const float SIZE_Y = 110.0f;
	const float SIZE_Z = 25.0f;
};