#include "TurretBase.h"

TurretBase::TurretBase(Game* game)
: Actor(game)
{
	SetScale(SCALE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretBase.gpmesh"));

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(SIZE_X, SIZE_Y, SIZE_Z);

	game->AddCollider(this);

	mHead = new TurretHead(game, this);

	mHealthComponent = new HealthComponent(this);
	mHealthComponent->SetOnDeathCallback([this] {
		Die();
	});
}

TurretBase::~TurretBase()
{
	GetGame()->RemoveCollider(this);
}

void TurretBase::Die()
{
	mHead->Die();
}