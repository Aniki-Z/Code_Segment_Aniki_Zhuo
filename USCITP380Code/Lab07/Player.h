#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include "HUD.h"

class Player : public Actor
{
private:
	class MeshComponent* mMesh;
	class CollisionComponent* mCollision;
	class PlayerMove* mPlayerMove;
	int mShield;
	class HUD* mHUD;

public:
	Player(Game* game);

	CollisionComponent* GetCollisionComponent() const { return mCollision; }
	int GetShield() const { return mShield; };
	void ReduceShield() { mShield--; };
	void AddShield()
	{
		mShield++;
		if (mShield > 3)
		{
			mShield = 3;
		}
	};
	PlayerMove* GetPlayerMove() { return mPlayerMove; };

	const float X_SIZE = 40.0f;
	const float Y_SIZE = 25.0f;
	const float Z_SIZE = 15.0f;
};
