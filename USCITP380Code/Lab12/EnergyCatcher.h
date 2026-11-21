#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Game.h"

class EnergyCatcher : public Actor
{
private:
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;
	std::string mDoorName;
	bool mActive;

public:
	EnergyCatcher(Game* game, std::string door);
	~EnergyCatcher();

	void OnUpdate(float deltaTime) override;

	bool GetActive() const { return mActive; }
	void SetActive(bool active) { mActive = active; }
};
