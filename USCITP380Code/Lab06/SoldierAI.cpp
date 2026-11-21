#include "SoldierAI.h"
#include "Actor.h"
#include "Game.h"
#include "PathFinder.h"
#include "PathNode.h"
#include "AnimatedSprite.h"
#include <algorithm>
#include <iostream>

SoldierAI::SoldierAI(Actor* owner)
: SpriteComponent(owner)
, mIsStunned(false)
, mStunTime(0.0f)
{
	mDirection = SoldierDirection::Down;
	SetIsVisible(false); // Uncomment to hide debug paths
}

void SoldierAI::Setup(PathNode* start, PathNode* end)
{
	mPatrolStart = start;
	mPatrolEnd = end;

	PathFinder* pathFinder = mOwner->GetGame()->GetPathFinder();
	pathFinder->CalculatePath(mPatrolStart, mPatrolEnd, mPath);

	mPrev = mPatrolStart;
	mNext = mPath.back();
	mPath.pop_back();
}

void SoldierAI::Update(float deltaTime)
{
	AnimatedSprite* animSprite = mOwner->GetComponent<AnimatedSprite>();
	if (mIsStunned)
	{
		mStunTime -= deltaTime;
		if (mStunTime <= 0.0f)
		{
			mIsStunned = false;
			animSprite->SetIsPaused(false);
		}
		return;
	}

	if (!mNext)
	{
		return;
	}

	Vector2 direction = mNext->GetPosition() - mOwner->GetPosition();
	float distance = direction.Length();

	if (mNext->GetPosition().x > mOwner->GetPosition().x)
	{
		mDirection = SoldierDirection::Right;
	}
	else if (mNext->GetPosition().x < mOwner->GetPosition().x)
	{
		mDirection = SoldierDirection::Left;
	}
	else if (mNext->GetPosition().y > mOwner->GetPosition().y)
	{
		mDirection = SoldierDirection::Down;
	}
	else if (mNext->GetPosition().y < mOwner->GetPosition().y)
	{
		mDirection = SoldierDirection::Up;
	}

	if (distance != 0.0f)
	{
		direction.x /= distance;
		direction.y /= distance;
	}

	Vector2 moveStep = direction * SOLDIER_SPEED * deltaTime;

	mOwner->SetPosition(mOwner->GetPosition() + moveStep);

	if ((mOwner->GetPosition() - mNext->GetPosition()).Length() <= 1.0f)
	{
		mOwner->SetPosition(mNext->GetPosition());

		if (!mPath.empty())
		{
			mPrev = mNext;
			mNext = mPath.back();
			mPath.pop_back();
		}
		else
		{
			Setup(mPatrolEnd, mPatrolStart);
		}
	}

	UpdateAnimation();
}

void SoldierAI::UpdateAnimation()
{
	AnimatedSprite* animSprite = mOwner->GetComponent<AnimatedSprite>();
	if (mDirection == SoldierDirection::Down)
	{
		animSprite->SetAnimation("WalkDown");
	}
	else if (mDirection == SoldierDirection::Up)
	{
		animSprite->SetAnimation("WalkUp");
	}
	else if (mDirection == SoldierDirection::Right)
	{
		animSprite->SetAnimation("WalkRight");
	}
	else if (mDirection == SoldierDirection::Left)
	{
		animSprite->SetAnimation("WalkLeft");
	}
}

// This helper is to just debug draw the soldier's path to visualize it
// (only called if this component is set to visible)
void SoldierAI::Draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();

	// Draw from current position to next
	SDL_RenderDrawLine(renderer, static_cast<int>(mOwner->GetPosition().x - cameraPos.x),
					   static_cast<int>(mOwner->GetPosition().y - cameraPos.y),
					   static_cast<int>(mNext->GetPosition().x - cameraPos.x),
					   static_cast<int>(mNext->GetPosition().y - cameraPos.y));

	// Draw from next to first node on path
	if (!mPath.empty())
	{
		// Draw from current position to next
		SDL_RenderDrawLine(renderer, static_cast<int>(mNext->GetPosition().x - cameraPos.x),
						   static_cast<int>(mNext->GetPosition().y - cameraPos.y),
						   static_cast<int>(mPath.back()->GetPosition().x - cameraPos.x),
						   static_cast<int>(mPath.back()->GetPosition().y - cameraPos.y));
	}

	// Draw each node on the path
	if (mPath.size() > 1)
	{
		for (size_t i = 0; i < mPath.size() - 1; i++)
		{
			SDL_RenderDrawLine(renderer, static_cast<int>(mPath[i]->GetPosition().x - cameraPos.x),
							   static_cast<int>(mPath[i]->GetPosition().y - cameraPos.y),
							   static_cast<int>(mPath[i + 1]->GetPosition().x - cameraPos.x),
							   static_cast<int>(mPath[i + 1]->GetPosition().y - cameraPos.y));
		}
	}
}
