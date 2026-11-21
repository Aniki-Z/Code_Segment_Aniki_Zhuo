#pragma once
#include "Game.h"

class EnergyCube : public Actor
{
private:
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;

public:
	EnergyCube(Game* game);
	~EnergyCube();
};
