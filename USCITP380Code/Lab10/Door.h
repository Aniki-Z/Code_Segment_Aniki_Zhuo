#pragma once
#include "Game.h"

class Door : public Actor
{
private:
	std::string mName;
	CollisionComponent* mCollisionComponent;
	MeshComponent* mMeshComponent;
	bool mIsOpen;

public:
	Door(Game* game, std::string name);
	~Door();
	void Open();

	const std::string& GetName() const { return mName; }
	bool IsOpen() const { return mIsOpen; }
};
