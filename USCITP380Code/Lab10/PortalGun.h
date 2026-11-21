#pragma once
#include "Game.h"

class PortalGun : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;
	class MeshComponent* mMeshComponent;

public:
	PortalGun(Game* game);
	void OnUpdate(float deltaTime) override;

	const float SIZE = 8.0f;
};
