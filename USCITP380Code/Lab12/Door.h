#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Game.h"

class Door : public Actor
{
private:
	std::string mName;
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;
	bool mIsOpen;

	Actor* mLeftDoor;
	Actor* mRightDoor;

	bool mIsOpening;
	float mOpenTime;

public:
	Door(Game* game, std::string name);
	~Door();
	void Open();

	void OnUpdate(float deltaTime) override;

	const std::string& GetName() const { return mName; }
	bool IsOpen() const { return mIsOpen; }

	const float OPEN_DURATION = 1.0f;
};
