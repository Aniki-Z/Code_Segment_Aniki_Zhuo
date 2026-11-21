#pragma once
#include "Actor.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"
#include "GoombaMove.h"

class Goomba : public Actor
{
private:
	class AnimatedSprite* mAnimatedSprite;
	class CollisionComponent* mCollisionComponent;
	class GoombaMove* mGoombaMove;

	bool mStomped;
	float mLifeTime;

public:
	Goomba(class Game* game);
	~Goomba();
	const CollisionComponent* GetCollisionComponent() const { return mCollisionComponent; }

	static constexpr float GOOMBA_SIZE = 32.0f;

	void Stomp();
	bool IsStomped() const { return mStomped; }

	void OnUpdate(float deltaTime) override;
};