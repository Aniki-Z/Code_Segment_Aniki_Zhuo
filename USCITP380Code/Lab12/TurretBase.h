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
};