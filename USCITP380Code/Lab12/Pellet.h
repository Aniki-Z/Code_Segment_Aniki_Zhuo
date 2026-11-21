#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Game.h"

class Pellet : public Actor
{
private:
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;

	Vector3 mVelocity;
	float mLifeTime;
	float mPostTeleportTime;

	bool mIsTeleport;
	bool mGreen;

public:
	Pellet(Game* game);

	void OnUpdate(float deltaTime) override;
	void SetVelocity(const Vector3& v);
	bool Teleport();
	bool Catcher();
	bool Turret();
};
