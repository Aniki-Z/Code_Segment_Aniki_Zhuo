#include "PlayerMove.h"
#include "Actor.h"
#include "Game.h"
#include "AnimatedSprite.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
, mVelocity(0.0f, 0.0f)
, mIsAttacking(false)
, mAttackTimer(0.0f)
{
	mDirection = Direction::None;
	mLastDirection = Direction::Down;
	mSword = new Sword(owner->GetGame());
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (mIsAttacking)
	{
		return;
	}

	Vector2 direction = Vector2(0.0f, 0.0f);

	if (mDirection != Direction::None)
	{
		mLastDirection = mDirection;
	}

	if (keyState[SDL_SCANCODE_W])
	{
		direction.y = -1.0f;
		mDirection = Direction::Up;
	}
	else if (keyState[SDL_SCANCODE_S])
	{
		direction.y = 1.0f;
		mDirection = Direction::Down;
	}
	else if (keyState[SDL_SCANCODE_A])
	{
		direction.x = -1.0f;
		mDirection = Direction::Left;
	}
	else if (keyState[SDL_SCANCODE_D])
	{
		direction.x = 1.0f;
		mDirection = Direction::Right;
	}
	else
	{
		mDirection = Direction::None;
	}

	if (direction.x != 0.0f || direction.y != 0.0f)
	{
		direction.Normalize();
		mVelocity = direction * SPEED;
	}
	else
	{
		mVelocity = Vector2(0.0f, 0.0f);
	}

	if (keyState[SDL_SCANCODE_SPACE] && !mIsAttacking && !mPrevKeyState[SDL_SCANCODE_SPACE])
	{
		mIsAttacking = true;
		mAttackTimer = 0.25f;

		GetGame()->GetAudio()->PlaySound("SwordSlash.wav");

		AnimatedSprite* animSprite = mOwner->GetGame()->GetPlayer()->GetAnimatedSprite();

		if (mDirection == Direction::Up || mLastDirection == Direction::Up)
		{
			animSprite->SetAnimation("AttackUp");
		}
		else if (mDirection == Direction::Down || mLastDirection == Direction::Down)
		{
			animSprite->SetAnimation("AttackDown");
		}
		else if (mDirection == Direction::Left || mLastDirection == Direction::Left)
		{
			animSprite->SetAnimation("AttackLeft");
		}
		else if (mDirection == Direction::Right || mLastDirection == Direction::Right)
		{
			animSprite->SetAnimation("AttackRight");
		}

		animSprite->ResetAnimTimer();
		mVelocity = Vector2(0.0f, 0.0f);
	}

	memcpy(mPrevKeyState, keyState, SDL_NUM_SCANCODES);
}

void PlayerMove::Update(float deltaTime)
{
	if (mIsAttacking)
	{
		mAttackTimer -= deltaTime;
		if (mAttackTimer <= 0.0f)
		{
			mIsAttacking = false;
		}
	}

	if (mIsAttacking)
	{
		const std::vector<EnemyComponent*>& enemies = mOwner->GetGame()->GetEnemies();
		for (EnemyComponent* enemy : enemies)
		{
			const CollisionComponent* enemyCollision = enemy->GetCollisionComponent();
			if (mSword->GetCollisionComponent()->Intersect(enemyCollision))
			{
				enemy->TakeDamage();
			}
		}
	}
	else
	{
		Vector2 position = mOwner->GetPosition();
		position += mVelocity * deltaTime;
		mOwner->SetPosition(position);

		CollisionComponent* playerCollision = mOwner->GetComponent<CollisionComponent>();

		const std::vector<Collider*>& colliders = mOwner->GetGame()->GetColliders();
		for (Collider* collider : colliders)
		{
			const CollisionComponent* colliderCollision = collider->GetCollisionComponent();
			Vector2 offset;

			CollSide side = playerCollision->GetMinOverlap(colliderCollision, offset);
			if (side != CollSide::None)
			{
				position += offset;
			}
			mOwner->SetPosition(position);
		}

		const std::vector<EnemyComponent*>& enemies = mOwner->GetGame()->GetEnemies();
		for (EnemyComponent* enemy : enemies)
		{
			const CollisionComponent* enemyCollision = enemy->GetCollisionComponent();
			Vector2 offset;

			CollSide side = playerCollision->GetMinOverlap(enemyCollision, offset);
			if (side != CollSide::None)
			{
				position += offset;
			}
			mOwner->SetPosition(position);
		}

		mOwner->GetGame()->SetCameraPos(position + Vector2(-256.0f, -224.0f));
	}

	UpdateSwordPosition();

	UpdateAnimation();
}

void PlayerMove::UpdateSwordPosition()
{
	if (!mIsAttacking)
	{
		mSword->SetPosition(Vector2(-1000, -1000));
		return;
	}

	Vector2 swordOffset;
	float swordWidth = 28.0f;
	float swordHeight = 28.0f;

	AnimatedSprite* animSprite = mOwner->GetGame()->GetPlayer()->GetAnimatedSprite();

	if (animSprite->GetAnimName() == "AttackUp")
	{
		swordOffset = Vector2(0.0f, -40.0f);
		swordWidth = 20.0f;
		swordHeight = 28.0f;
	}
	else if (animSprite->GetAnimName() == "AttackDown")
	{
		swordOffset = Vector2(0.0f, 40.0f);
		swordWidth = 20.0f;
		swordHeight = 28.0f;
	}
	else if (animSprite->GetAnimName() == "AttackLeft")
	{
		swordOffset = Vector2(-32.0f, 0.0f);
		swordWidth = 28.0f;
		swordHeight = 20.0f;
	}
	else if (animSprite->GetAnimName() == "AttackRight")
	{
		swordOffset = Vector2(32.0f, 0.0f);
		swordWidth = 28.0f;
		swordHeight = 20.0f;
	}
	else
	{
		return;
	}

	mSword->SetPosition(mOwner->GetPosition() + swordOffset);
	mSword->SetCollisionSize(swordWidth, swordHeight);
}

void PlayerMove::UpdateAnimation()
{
	AnimatedSprite* animSprite = mOwner->GetGame()->GetPlayer()->GetAnimatedSprite();

	if (mIsAttacking)
	{
		return;
	}

	if (mDirection == Direction::Up)
	{
		animSprite->SetAnimation("WalkUp");
	}
	else if (mDirection == Direction::Down)
	{
		animSprite->SetAnimation("WalkDown");
	}
	else if (mDirection == Direction::Left)
	{
		animSprite->SetAnimation("WalkLeft");
	}
	else if (mDirection == Direction::Right)
	{
		animSprite->SetAnimation("WalkRight");
	}
	else if (mDirection == Direction::None)
	{
		if (mLastDirection == Direction::Up)
		{
			animSprite->SetAnimation("StandUp");
		}
		else if (mLastDirection == Direction::Down)
		{
			animSprite->SetAnimation("StandDown");
		}
		else if (mLastDirection == Direction::Left)
		{
			animSprite->SetAnimation("StandLeft");
		}
		else if (mLastDirection == Direction::Right)
		{
			animSprite->SetAnimation("StandRight");
		}
	}
}
