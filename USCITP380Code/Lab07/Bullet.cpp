#include "Bullet.h"

Bullet::Bullet(Game* game)
: Actor(game)
, mAliveTime(1.0f)
{
	SetScale(BULLET_SCALE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(BULLET_SIZE, BULLET_SIZE, BULLET_SIZE);

	mMoveComponent = new MoveComponent(this);
	mMoveComponent->SetForwardSpeed(BULLET_SPEED);
}

void Bullet::OnUpdate(float deltaTime)
{
	mMoveComponent->SetForwardSpeed(BULLET_SPEED *
									GetGame()->GetPlayer()->GetPlayerMove()->GetSpeedMultiplier());

	mAliveTime -= deltaTime;

	if (mAliveTime <= 0.0f)
	{
		SetState(ActorState::Destroy);
	}

	const std::vector<Block*>& blocks = GetGame()->GetBlocks();
	for (Block* block : blocks)
	{
		const CollisionComponent* blockCollision = block->GetCollisionComponent();

		if (mCollisionComponent->Intersect(blockCollision))
		{
			if (block->CanExplode())
			{
				GetGame()->GetAudio()->PlaySound("BlockExplode.wav");
				Explode(block);
			}
			SetState(ActorState::Destroy);
		}
	}
}

void Bullet::Explode(Block* block)
{
	block->SetState(ActorState::Destroy);
	block->SetExplode(true);

	const std::vector<Block*>& blocks = GetGame()->GetBlocks();
	for (Block* otherBlock : blocks)
	{
		if (Vector3::Distance(block->GetPosition(), otherBlock->GetPosition()) <= EXPLODE_RADIUS &&
			!otherBlock->GetExplode())
		{
			if (otherBlock != block && otherBlock->CanExplode())
			{
				Explode(otherBlock);
			}
			else
			{
				otherBlock->SetState(ActorState::Destroy);
			}
		}
	}
}