#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

class GhostAI : public Component
{
public:
	// Used to track the four different GhostAI states
	enum State
	{
		Scatter,
		Chase,
		Frightened,
		Dead
	};

	GhostAI(class Actor* owner);

	void Update(float deltaTime) override;

	// Called when the Ghost starts at the beginning
	// (or when the ghosts should respawn)
	void Start(class PathNode* startNode);

	// Get the current state
	State GetState() const { return mState; }

	// Called when the ghost should switch to the "Frightened" state
	void Frighten();

	// Called when the ghost should switch to the "Dead" state
	void Die();

	//  Helper function to draw GhostAI's current goal, prev, and next
	void DebugDrawPath(struct SDL_Renderer* render);

private:
	// Member data for pathfinding

	// TargetNode is our current goal node
	class PathNode* mTargetNode = nullptr;
	// PrevNode is the last node we intersected
	// with prior to the current position
	class PathNode* mPrevNode = nullptr;
	// NextNode is the next node we're trying to get to
	class PathNode* mNextNode = nullptr;

	// Current state of the Ghost AI
	State mState = Scatter;

	// Save the owning actor (cast to a Ghost*)
	class Ghost* mGhost;

	// TODO: Add any member data/helper functions here!
	void IntersectWithNextNode();
	void UpdateTargetNode();
	void SelectNextNode();
	void UpdateMoveDirection();
	void BlinkyChase();
	void PinkyChase();
	void InkyChase();
	void ClydeChase();

	Vector2 mMoveDirection;
	float mStateTime = 0.0f;
	float mMoveSpeed = 0.0f;

	static constexpr float SCATTER_CHASE_SPEED = 90.0f;
	static constexpr float FRIGHTENED_SPEED = 65.0f;
	static constexpr float DEAD_SPEED = 125.0f;
	static constexpr float FRONT_OF_PINKY = 80.0f;
	static constexpr float FRONT_OF_INKY = 40.0f;
	static constexpr float MAX_CLYDE_DISTANCE = 150.0f;
};
