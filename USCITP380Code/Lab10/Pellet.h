#pragma once
#include "Game.h"

class Pellet : public Actor
{
private:
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;

	Vector3 mVelocity;
	float mLifeTime;

	bool mIsTeleport;
	bool mGreen;

public:
	Pellet(Game* game);

	void OnUpdate(float deltaTime) override;
	void SetVelocity(const Vector3& v);
	bool Teleport();
	bool Catcher();

	const float LIFE_TIME = 0.25f;
	const float FORWARD_SPEED = 40.0f;
};
