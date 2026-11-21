#include "PlayerMove.h"
#include "Actor.h"
#include "Game.h"
#include "AnimatedSprite.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
, mForwardSpeed(0.0f)
, mYSpeed(0.0f)
, mSpacePressed(false)
, mInAir(false)
{
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		mForwardSpeed = -MOVE_SPEED;
	}
	else if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		mForwardSpeed = MOVE_SPEED;
	}
	else
	{
		mForwardSpeed = 0.0f;
	}

	if (keyState[SDL_SCANCODE_SPACE] && !mSpacePressed && !mInAir)
	{
		mYSpeed = JUMP_SPEED;
		mInAir = true;
		GetGame()->PlaySound("Assets/Sounds/Jump.wav");
	}

	mSpacePressed = keyState[SDL_SCANCODE_SPACE];
}

void PlayerMove::Update(float deltaTime)
{
	Vector2 position = mOwner->GetPosition();
	position.x += mForwardSpeed * deltaTime;
	position.y += mYSpeed * deltaTime;

	Player* player = static_cast<Player*>(mOwner);
	AnimatedSprite* animSprite = player->GetAnimatedSprite();
	std::string currentAnim = animSprite->GetAnimName();

	mInAir = true;

	if (position.x <= mOwner->GetGame()->GetCameraPos().x)
	{
		position.x = mOwner->GetGame()->GetCameraPos().x;
	}

	if (position.y > Game::WINDOW_HEIGHT)
	{
		Mix_HaltChannel(GetGame()->GetMusicChannel());
		mOwner->GetGame()->PlaySound("Assets/Sounds/Dead.wav");
		mOwner->GetComponent<SpriteComponent>()->SetTexture(
			mOwner->GetGame()->GetTexture("Assets/Mario/Dead.png"));
		mOwner->SetState(ActorState::Paused);
		return;
	}

	if (position.x > WIN_LOCATION)
	{
		Mix_HaltChannel(GetGame()->GetMusicChannel());
		mOwner->GetGame()->PlaySound("Assets/Sounds/StageClear.wav");
		mOwner->SetState(ActorState::Paused);
		return;
	}

	if (position.y >= GROUND_LOCATION)
	{
		position.y = GROUND_LOCATION;
		mYSpeed = 0.0f;
		mInAir = false;
	}

	const std::vector<Block*>& blocks = mOwner->GetGame()->GetBlocks();
	for (Block* block : blocks)
	{
		const CollisionComponent* blockCollision = block->GetCollisionComponent();
		Vector2 offset;

		CollSide side = mOwner->GetComponent<CollisionComponent>()->GetMinOverlap(blockCollision,
																				  offset);
		if (side != CollSide::None)
		{
			if (side == CollSide::Top && mYSpeed > 0.0f)
			{
				mYSpeed = 0.0f;
				position.y += offset.y;
				mInAir = false;
			}
			else if (side == CollSide::Bottom && mYSpeed < 0.0f)
			{
				mYSpeed = 0.0f;
				GetGame()->PlaySound("Assets/Sounds/Bump.wav");
			}
			else if (side == CollSide::Left || side == CollSide::Right)
			{
				position += offset;
			}

			mOwner->SetPosition(position);
		}
	}

	std::vector<Goomba*>& goombas = mOwner->GetGame()->GetGoombas();
	for (Goomba* goomba : goombas)
	{
		const CollisionComponent* goombaCollision = goomba->GetCollisionComponent();
		Vector2 offset;

		CollSide side = mOwner->GetComponent<CollisionComponent>()->GetMinOverlap(goombaCollision,
																				  offset);
		if (side == CollSide::Top || (side != CollSide::None && mInAir))
		{
			goomba->Stomp();
			mYSpeed = AFTER_STOMP_SPEED;
			mInAir = true;
		}
		else if (side != CollSide::None)
		{
			animSprite->SetAnimation("dead");
			Mix_HaltChannel(GetGame()->GetMusicChannel());
			mOwner->GetGame()->PlaySound("Assets/Sounds/Dead.wav");
			mOwner->SetState(ActorState::Paused);
			return;
		}
	}

	mYSpeed += GRAVITY * deltaTime;

	mOwner->SetPosition(position);

	Vector2& cameraPos = mOwner->GetGame()->GetCameraPos();
	float newCameraPosX = position.x - HALF_SCREEN_LOCATION;
	if (newCameraPosX >= cameraPos.x)
	{
		cameraPos.x = newCameraPosX;
	}
	if (cameraPos.x <= 0)
	{
		cameraPos.x = 0;
	}

	UpdateAnimation();
}

void PlayerMove::UpdateAnimation()
{
	AnimatedSprite* animSprite = mOwner->GetGame()->GetPlayer()->GetAnimatedSprite();
	std::string currentAnim = animSprite->GetAnimName();
	bool movingRight = mForwardSpeed > 0;
	bool movingLeft = mForwardSpeed < 0;

	if (!mInAir)
	{
		if (movingRight)
			animSprite->SetAnimation("runRight");
		else if (movingLeft)
			animSprite->SetAnimation("runLeft");
		else
			animSprite->SetAnimation("idle");
	}
	else
	{
		if (movingRight)
			animSprite->SetAnimation("jumpRight");
		else if (movingLeft)
			animSprite->SetAnimation("jumpLeft");
		else
		{
			if (currentAnim == "runRight" || currentAnim == "jumpRight" || currentAnim == "idle")
				animSprite->SetAnimation("jumpRight");
			else
				animSprite->SetAnimation("jumpLeft");
		}
	}
}
