#pragma once
#include "Game.h"

class Prop : public Actor
{
private:
	class CollisionComponent* mCollisionComponent;
	class MeshComponent* mMeshComponent;
	bool mHasCollision;
	std::string mMeshFile;

public:
	Prop(Game* game, const std::string& meshFile, bool usesAlpha, bool hasCollision);
	~Prop();

	void SetMesh(const std::string& meshFile);
	void SetUsesAlpha(bool usesAlpha);
	void SetHasCollision(bool hasCollision);
};
