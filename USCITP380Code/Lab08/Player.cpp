#include "Player.h"

Player::Player(Game* game)
: Actor(game)
{
	SetScale(0.75f);

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));

	mPlayerMove = new PlayerMove(this);

	mCameraComponent = new CameraComponent(this);
	mCameraComponent->SnapToIdeal();

	mPlayerUI = new PlayerUI(this);
}