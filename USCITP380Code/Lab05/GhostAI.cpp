#include "GhostAI.h"
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "PathNode.h"
#include "AnimatedSprite.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include "Ghost.h"
#include "PacMan.h"
#include "Random.h"

GhostAI::GhostAI(class Actor* owner)
: Component(owner, 50)
, mMoveSpeed(SCATTER_CHASE_SPEED)
{
	mGhost = static_cast<Ghost*>(owner);
}

void GhostAI::Update(float deltaTime)
{
	if (!mNextNode)
	{
		return;
	}

	mStateTime += deltaTime;

	if (mState == Frightened && mStateTime >= 7.0f)
	{
		mState = Scatter;
		mStateTime = 0.0f;
	}

	if (mState == Dead && mNextNode == GetGame()->GetGhostPen())
	{
		mState = Scatter;
		mStateTime = 0.0f;
	}

	if (mState == Scatter && mStateTime >= 5.0f)
	{
		mState = Chase;
		mStateTime = 0.0f;
	}
	else if (mState == Chase && mStateTime >= 20.0f)
	{
		mState = Scatter;
		mStateTime = 0.0f;
	}

	mMoveSpeed = SCATTER_CHASE_SPEED;
	if (mState == Frightened)
	{
		mMoveSpeed = FRIGHTENED_SPEED;
	}
	else if (mState == Dead)
	{
		mMoveSpeed = DEAD_SPEED;
	}

	mGhost->SetPosition(mGhost->GetPosition() + mMoveDirection * mMoveSpeed * deltaTime);

	if (Vector2::Distance(mGhost->GetPosition(), mNextNode->GetPosition()) < 2.0f)
	{
		IntersectWithNextNode();
	}
}

void GhostAI::IntersectWithNextNode()
{
	mGhost->SetPosition(mNextNode->GetPosition());

	UpdateTargetNode();

	SelectNextNode();
}

void GhostAI::UpdateTargetNode()
{
	if (mState == Scatter)
	{
		mTargetNode = mGhost->GetScatterNode();
		mMoveSpeed = SCATTER_CHASE_SPEED;
	}
	else if (mState == Frightened)
	{
		std::vector<PathNode*> validNodes;

		for (size_t i = 0; i < mNextNode->mAdjacent.size(); i++)
		{
			PathNode* adjNode = mNextNode->mAdjacent[i];
			if (adjNode != mPrevNode && adjNode->GetType() != PathNode::Ghost &&
				adjNode->GetType() != PathNode::Tunnel)
			{
				validNodes.push_back(adjNode);
			}
		}

		if (validNodes.empty())
		{
			for (size_t i = 0; i < mNextNode->mAdjacent.size(); i++)
			{
				PathNode* adjNode = mNextNode->mAdjacent[i];
				if (adjNode != mPrevNode && adjNode->GetType() != PathNode::Tunnel)
				{
					validNodes.push_back(adjNode);
				}
			}
		}

		if (validNodes.empty())
		{
			validNodes = mNextNode->mAdjacent;
		}

		if (!validNodes.empty())
		{
			int randomIndex = Random::GetIntRange(0, static_cast<int>(validNodes.size()) - 1);
			mTargetNode = validNodes[randomIndex];
		}

		mMoveSpeed = FRIGHTENED_SPEED;
	}
	else if (mState == Chase)
	{
		if (mGhost->GetType() == Ghost::Blinky)
		{
			BlinkyChase();
		}
		else if (mGhost->GetType() == Ghost::Pinky)
		{
			PinkyChase();
		}
		else if (mGhost->GetType() == Ghost::Inky)
		{
			InkyChase();
		}
		else if (mGhost->GetType() == Ghost::Clyde)
		{
			ClydeChase();
		}
	}
	else
	{
		mMoveSpeed = DEAD_SPEED;
	}
}

void GhostAI::BlinkyChase()
{
	PathNode* pacManNode = GetGame()->GetPlayer()->GetPrevNode();
	if (pacManNode && pacManNode->GetType() == PathNode::Tunnel)
	{
		PathNode* nearNode = nullptr;
		float minDis = std::numeric_limits<float>::max();

		for (PathNode* node : GetGame()->GetPathNodes())
		{
			if (node->GetType() == PathNode::Default)
			{
				float dist = Vector2::Distance(pacManNode->GetPosition(), node->GetPosition());
				if (dist < minDis)
				{
					nearNode = node;
					minDis = dist;
				}
			}
		}
		mTargetNode = nearNode;
	}
	else
	{
		mTargetNode = pacManNode;
	}
}

void GhostAI::PinkyChase()
{
	Vector2 pacManFront = GetGame()->GetPlayer()->GetPointInFrontOf(FRONT_OF_PINKY);
	PathNode* nearNode = nullptr;
	float minDis = std::numeric_limits<float>::max();

	for (PathNode* node : GetGame()->GetPathNodes())
	{
		if (node->GetType() == PathNode::Default)
		{
			float dist = Vector2::Distance(pacManFront, node->GetPosition());
			if (dist < minDis)
			{
				nearNode = node;
				minDis = dist;
			}
		}
	}
	mTargetNode = nearNode;
}

void GhostAI::InkyChase()
{
	Vector2 pacManFront = GetGame()->GetPlayer()->GetPointInFrontOf(FRONT_OF_INKY);
	Vector2 blinkyPos = GetGame()->GetGhosts()[0]->GetPosition();
	Vector2 inkyDir = pacManFront - blinkyPos;
	inkyDir *= 2.0f;
	Vector2 pointQ = blinkyPos + inkyDir;

	PathNode* nearNode = nullptr;
	float minDis = std::numeric_limits<float>::max();

	for (PathNode* node : GetGame()->GetPathNodes())
	{
		if (node->GetType() == PathNode::Default)
		{
			float dist = Vector2::Distance(pointQ, node->GetPosition());
			if (dist < minDis)
			{
				nearNode = node;
				minDis = dist;
			}
		}
	}
	mTargetNode = nearNode;
}

void GhostAI::ClydeChase()
{
	float clydeDis = Vector2::Distance(mGhost->GetPosition(),
									   GetGame()->GetPlayer()->GetPosition());
	if (clydeDis > MAX_CLYDE_DISTANCE)
	{
		PathNode* pacPrevNode = GetGame()->GetPlayer()->GetPrevNode();
		if (pacPrevNode)
		{
			mTargetNode = pacPrevNode;
		}
	}
	else
	{
		mTargetNode = mGhost->GetScatterNode();
	}
}

void GhostAI::SelectNextNode()
{
	if (!mTargetNode)
	{
		UpdateTargetNode();
		if (!mTargetNode)
		{
			return;
		}
	}

	PathNode* goodNode = nullptr;
	float minDis = std::numeric_limits<float>::max();

	for (size_t i = 0; i < mNextNode->mAdjacent.size(); i++)
	{
		PathNode* adjNode = mNextNode->mAdjacent[i];

		if ((adjNode == mPrevNode) || (adjNode->GetType() == PathNode::Tunnel) ||
			(mState != Dead && adjNode->GetType() == PathNode::Ghost))
		{
			continue;
		}

		float dist = Vector2::Distance(adjNode->GetPosition(), mTargetNode->GetPosition());
		if (dist < minDis)
		{
			minDis = dist;
			goodNode = adjNode;
		}
	}

	if (!goodNode)
	{
		for (size_t i = 0; i < mNextNode->mAdjacent.size(); i++)
		{
			PathNode* adjNode = mNextNode->mAdjacent[i];

			if ((adjNode == mPrevNode) || (adjNode->GetType() == PathNode::Tunnel) ||
				(mState != Dead && adjNode->GetType() == PathNode::Ghost))
			{
				continue;
			}

			float dist = Vector2::Distance(adjNode->GetPosition(), mTargetNode->GetPosition());
			if (dist < minDis)
			{
				minDis = dist;
				goodNode = adjNode;
			}
		}
	}

	if (!goodNode)
	{
		for (size_t i = 0; i < mNextNode->mAdjacent.size(); i++)
		{
			PathNode* adjNode = mNextNode->mAdjacent[i];

			float dist = Vector2::Distance(adjNode->GetPosition(), mTargetNode->GetPosition());
			if (dist < minDis)
			{
				minDis = dist;
				goodNode = adjNode;
			}
		}
	}

	if (goodNode)
	{
		mPrevNode = mNextNode;
		mNextNode = goodNode;
		UpdateMoveDirection();
	}
}

void GhostAI::UpdateMoveDirection()
{
	if (mPrevNode && mNextNode)
	{
		mMoveDirection = mNextNode->GetPosition() - mPrevNode->GetPosition();
		mMoveDirection.Normalize();

		AnimatedSprite* ghostMoveAnime = mGhost->GetComponent<AnimatedSprite>();
		if (mState == Scatter || mState == Chase)
		{
			if (mMoveDirection.x > 0)
			{
				ghostMoveAnime->SetAnimation("right");
			}
			else if (mMoveDirection.x < 0)
			{
				ghostMoveAnime->SetAnimation("left");
			}
			else if (mMoveDirection.y > 0)
			{
				ghostMoveAnime->SetAnimation("down");
			}
			else if (mMoveDirection.y < 0)
			{
				ghostMoveAnime->SetAnimation("up");
			}
		}
		else if (mState == Frightened)
		{
			if (mStateTime <= 5.0f)
			{
				ghostMoveAnime->SetAnimation("scared0");
			}
			else if (mStateTime > 5.0f && mStateTime <= 7.0f)
			{
				ghostMoveAnime->SetAnimation("scared1");
			}
			else if (mStateTime > 7.0f)
			{
				mStateTime = 0.0f;
				mState = Scatter;
			}
		}
		else if (mState == Dead)
		{
			if (mMoveDirection.y < 0)
			{
				ghostMoveAnime->SetAnimation("deadup");
			}
			else if (mMoveDirection.y > 0)
			{
				ghostMoveAnime->SetAnimation("deaddown");
			}
			else if (mMoveDirection.x < 0)
			{
				ghostMoveAnime->SetAnimation("deadleft");
			}
			else if (mMoveDirection.x > 0)
			{
				ghostMoveAnime->SetAnimation("deadright");
			}
		}
	}
}

void GhostAI::Frighten()
{
	if (mState == Dead)
	{
		return;
	}

	mStateTime = 0.0f;

	if (mState != Frightened)
	{
		mState = Frightened;

		std::swap(mPrevNode, mNextNode);
		UpdateMoveDirection();

		mTargetNode = nullptr;
	}
}

void GhostAI::Die()
{
	if (mState == Dead)
	{
		return;
	}

	mStateTime = 0.0f;
	mState = Dead;

	UpdateMoveDirection();

	mTargetNode = GetGame()->GetGhostPen();
}

void GhostAI::Start(PathNode* startNode)
{
	mState = Scatter;
	mPrevNode = nullptr;
	mNextNode = startNode;
	mGhost->SetPosition(startNode->GetPosition());

	UpdateTargetNode();
}

void GhostAI::DebugDrawPath(SDL_Renderer* render)
{
	// Draw a rectangle at the target node
	if (mTargetNode != nullptr)
	{
		const int SIZE = 16;
		SDL_Rect r;
		r.x = static_cast<int>(mTargetNode->GetPosition().x) - SIZE / 2;
		r.y = static_cast<int>(mTargetNode->GetPosition().y) - SIZE / 2;
		r.w = SIZE;
		r.h = SIZE;
		SDL_RenderDrawRect(render, &r);
	}

	// Line from ghost to next node
	if (mNextNode != nullptr)
	{
		SDL_RenderDrawLine(render, static_cast<int>(mOwner->GetPosition().x),
						   static_cast<int>(mOwner->GetPosition().y),
						   static_cast<int>(mNextNode->GetPosition().x),
						   static_cast<int>(mNextNode->GetPosition().y));
	}
}
