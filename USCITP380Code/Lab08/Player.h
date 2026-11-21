#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include "Mesh.h"
#include "CameraComponent.h"
#include "PlayerUI.h"

class Player : public Actor
{
private:
	class MeshComponent* mMesh;
	class PlayerMove* mPlayerMove;
	class CameraComponent* mCameraComponent;
	class PlayerUI* mPlayerUI;

public:
	Player(Game* game);

	PlayerMove* GetPlayerMove() { return mPlayerMove; };
	PlayerUI* GetPlayerUI() { return mPlayerUI; };
};
