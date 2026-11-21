#include "GoombaMove.h"
#include "Game.h"

GoombaMove::GoombaMove(Actor* owner)
: MoveComponent(owner)
, mDirection(-1.0f)
, mVelocityY(0.0f)
{
}

void GoombaMove::Update(float deltaTime)
{
	if (static_cast<Goomba*>(mOwner)->IsStomped())
	{
		return;
	}

	Vector2 goombaPos = mOwner->GetPosition();
	goombaPos.x += mDirection * SPEED * deltaTime;
	goombaPos.y += mVelocityY * deltaTime;
	mOwner->SetPosition(goombaPos);

	const std::vector<Block*>& blocks = GetGame()->GetBlocks();
	for (size_t i = 0; i < blocks.size(); i++)
	{
		Block* block = blocks[i];
		const CollisionComponent* mCollision = block->GetCollisionComponent();
		Vector2 offset;

		CollSide mSide = mOwner->GetComponent<CollisionComponent>()->GetMinOverlap(mCollision,
																				   offset);

		if (mSide == CollSide::Top)
		{
			mVelocityY = 0.0f;
			goombaPos.y += offset.y;
		}

		mOwner->SetPosition(goombaPos);
	}

	for (size_t i = 0; i < blocks.size(); i++)
	{
		Block* mBlock = blocks[i];
		const CollisionComponent* mCollision = mBlock->GetCollisionComponent();
		Vector2 offset;

		CollSide mSide = mOwner->GetComponent<CollisionComponent>()->GetMinOverlap(mCollision,
																				   offset);

		if (mSide == CollSide::Left || mSide == CollSide::Right)
		{
			mDirection *= -1;
			goombaPos.x += offset.x;
		}

		mOwner->SetPosition(goombaPos);
	}

	const std::vector<Goomba*>& goombas = GetGame()->GetGoombas();
	for (size_t i = 0; i < goombas.size(); i++)
	{
		Goomba* mGoomba = goombas[i];
		const CollisionComponent* mCollision = mGoomba->GetCollisionComponent();
		Vector2 offset;

		CollSide mSide = mOwner->GetComponent<CollisionComponent>()->GetMinOverlap(mCollision,
																				   offset);

		if (mOwner != mGoomba && mSide != CollSide::None)
		{
			mDirection *= -1;
			goombaPos.x += offset.x;
		}

		mOwner->SetPosition(goombaPos);
	}

	if (goombaPos.y >= Game::WINDOW_HEIGHT)
	{
		mOwner->SetState(ActorState::Destroy);
	}

	mVelocityY += GRAVITY * deltaTime;
	mOwner->SetPosition(goombaPos);
}