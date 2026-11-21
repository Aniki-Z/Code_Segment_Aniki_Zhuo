#include "Player.h"

Player::Player(Game* game)
: Actor(game)
, mShield(3)
{
	SetScale(2.0f);

	mMesh = new MeshComponent(this);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Arwing.gpmesh"));

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(X_SIZE, Y_SIZE, Z_SIZE);

	mPlayerMove = new PlayerMove(this);

	mHUD = new HUD(this);
}
