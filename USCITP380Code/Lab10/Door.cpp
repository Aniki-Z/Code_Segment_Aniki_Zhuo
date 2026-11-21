#include "Door.h"
#include "Mesh.h"

Door::Door(Game* game, std::string name)
: Actor(game)
, mName(name)
, mIsOpen(false)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/DoorFrame.gpmesh");

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(mMesh);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(mMesh->GetWidth(), mMesh->GetHeight(), mMesh->GetDepth());

	GetGame()->AddCollider(this);
}

Door::~Door()
{
	GetGame()->RemoveCollider(this);
}

void Door::Open()
{
	if (!mIsOpen)
	{
		mIsOpen = true;
		GetGame()->RemoveCollider(this);
	}
}
