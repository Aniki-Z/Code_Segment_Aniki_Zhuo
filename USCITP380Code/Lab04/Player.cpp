#include "Player.h"

Player::Player(Game* game)
: Actor(game)
{
	mAnimatedSprite = new AnimatedSprite(this, 200);

	mAnimatedSprite->AddAnimation("idle", {game->GetTexture("Assets/Mario/Idle.png")});
	mAnimatedSprite->AddAnimation("dead", {game->GetTexture("Assets/Mario/Dead.png")});
	mAnimatedSprite->AddAnimation("jumpLeft", {game->GetTexture("Assets/Mario/JumpLeft.png")});
	mAnimatedSprite->AddAnimation("jumpRight", {game->GetTexture("Assets/Mario/JumpRight.png")});

	mAnimatedSprite->AddAnimation("runLeft", {game->GetTexture("Assets/Mario/RunLeft0.png"),
											  game->GetTexture("Assets/Mario/RunLeft1.png"),
											  game->GetTexture("Assets/Mario/RunLeft2.png")});

	mAnimatedSprite->AddAnimation("runRight", {game->GetTexture("Assets/Mario/RunRight0.png"),
											   game->GetTexture("Assets/Mario/RunRight1.png"),
											   game->GetTexture("Assets/Mario/RunRight2.png")});

	mAnimatedSprite->SetAnimation("idle");

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(PLAYER_SIZE, PLAYER_SIZE);

	mPlayerMove = new PlayerMove(this);
}