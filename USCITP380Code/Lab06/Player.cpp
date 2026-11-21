#include "Player.h"

Player::Player(Game* game)
: Actor(game)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(PLAYER_SIZE, PLAYER_SIZE);

	mAnimatedSprite = new AnimatedSprite(this, 200);
	mAnimatedSprite->LoadAnimations("Assets/Link");
	mAnimatedSprite->SetAnimation("StandDown");

	mPlayerMove = new PlayerMove(this);
}