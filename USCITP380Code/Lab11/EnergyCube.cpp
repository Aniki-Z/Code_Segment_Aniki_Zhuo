#include "EnergyCube.h"
#include "Mesh.h"

EnergyCube::EnergyCube(Game* game)
: Actor(game)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/EnergyCube.gpmesh");

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(mMesh);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(25.0f, 25.0f, 25.0f);

	GetGame()->AddCollider(this);
}

EnergyCube::~EnergyCube()
{
	GetGame()->RemoveCollider(this);
}