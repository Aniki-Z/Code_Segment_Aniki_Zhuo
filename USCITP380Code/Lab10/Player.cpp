#include "Player.h"

Player::Player(Game* game)
: Actor(game)
{
	Actor::SetScale(1.0f);

	mPlayerMove = new PlayerMove(this);

	mCameraComponent = new CameraComponent(this);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(50, 100, 50);

	mHasGun = false;
}

void Player::HasGunNow()
{
	mHasGun = true;
	new PlayerMesh(GetGame());
}