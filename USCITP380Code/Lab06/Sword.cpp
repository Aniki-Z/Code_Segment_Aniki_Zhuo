#include "Sword.h"

Sword::Sword(Game* game)
: Actor(game)
{
	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(SWORD_SIZE, SWORD_SIZE);
}

void Sword::SetCollisionSize(float width, float height)
{
	mCollisionComponent->SetSize(width, height);
}