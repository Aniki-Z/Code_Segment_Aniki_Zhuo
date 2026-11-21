#include "Goomba.h"
#include "Game.h"
#include <chrono>

Goomba::Goomba(Game* game)
: Actor(game)
, mStomped(false)
, mLifeTime(0.25f)
{
	mAnimatedSprite = new AnimatedSprite(this, 150);

	std::vector<SDL_Texture*> walkAnim{game->GetTexture("Assets/Goomba/Walk0.png"),
									   game->GetTexture("Assets/Goomba/Walk1.png")};
	mAnimatedSprite->AddAnimation("walk", walkAnim);

	std::vector<SDL_Texture*> deadAnim{game->GetTexture("Assets/Goomba/Dead.png")};
	mAnimatedSprite->AddAnimation("dead", deadAnim);

	mAnimatedSprite->SetAnimation("walk");

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(GOOMBA_SIZE, GOOMBA_SIZE);

	mGoombaMove = new GoombaMove(this);
}

Goomba::~Goomba()
{
	GetGame()->RemoveEnemy(this);
}

void Goomba::Stomp()
{
	mStomped = true;
	mAnimatedSprite->SetAnimation("dead");
	mCollisionComponent->SetSize(0, 0);
	GetGame()->PlaySound("Assets/Sounds/Stomp.wav");
}

void Goomba::OnUpdate(float deltaTime)
{
	Actor::OnUpdate(deltaTime);

	if (mStomped)
	{
		mLifeTime -= deltaTime;
		if (mLifeTime <= 0.0f)
		{
			SetState(ActorState::Destroy);
		}
	}
}