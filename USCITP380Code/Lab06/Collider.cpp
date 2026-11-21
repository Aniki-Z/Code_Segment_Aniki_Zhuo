#include "Collider.h"

Collider::Collider(Game* game, int width, int height)
: Actor(game)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(width, height);
}