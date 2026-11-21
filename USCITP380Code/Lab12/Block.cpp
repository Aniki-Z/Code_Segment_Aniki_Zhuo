#include "Block.h"

Block::Block(Game* game)
: Actor(game)
{
	Actor::SetScale(SCALE);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(SIZE, SIZE, SIZE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh"));

	GetGame()->AddCollider(this);
}

Block::~Block()
{
	GetGame()->RemoveCollider(this);
}