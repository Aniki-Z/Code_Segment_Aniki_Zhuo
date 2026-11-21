#pragma once
#include "Actor.h"
#include "CollisionComponent.h"

class Portal : public Actor
{
private:
	void CalcViewMatrix(struct PortalData& portalData, Portal* exitPortal);
	bool mIsBlue;
	class CollisionComponent* mCollisionComponent;

public:
	Portal(class Game* game, bool isBlue);

	Vector3 GetPortalOutVector(const Vector3& initialVector, Portal* portal, float w);

	void OnUpdate(float deltaTime) override;

	class CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }
};
