#include "EnergyGlass.h"
#include "Mesh.h"

EnergyGlass::EnergyGlass(Game* game)
: Actor(game)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh");

	mMeshComponent = new MeshComponent(this, true);
	mMeshComponent->SetMesh(mMesh);
	mMeshComponent->SetTextureIndex(17);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(1.0f, 1.0f, 1.0f);

	GetGame()->AddCollider(this);
}

EnergyGlass::~EnergyGlass()
{
	GetGame()->RemoveCollider(this);
}