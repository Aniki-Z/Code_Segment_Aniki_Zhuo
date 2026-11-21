#include "TurretBase.h"

TurretBase::TurretBase(Game* game)
: Actor(game)
{
	SetScale(SCALE);

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretBase.gpmesh"));

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(25.0f, 110.0f, 25.0f);

	game->AddCollider(this);

	mHead = new TurretHead(game, this);

	mHealthComponent = new HealthComponent(this);
	mHealthComponent->SetOnDeathCallback([this] {
		Die();
	});

	mHealthComponent->SetOnDamageCallback([this](const Vector3& location) {
		mHead->TakeDamage();
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