#pragma once
#include "Game.h"
#include "Pellet.h"

class EnergyLauncher : public Actor
{
private:
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;
	float mCooldown;
	float mTimer;
	std::string mDoorName;
	Pellet* mPellet;
	bool mDoorOpen;

public:
	EnergyLauncher(Game* game, std::string door, float coolDown = 1.5f);
	~EnergyLauncher();
	void OnUpdate(float deltaTime) override;

	const std::string& GetName() const { return mDoorName; }
	void SetOpen(bool open) { mDoorOpen = open; }

	const float SPEED = 500.0f;
	const float SPAWN = 20.0f;
};
