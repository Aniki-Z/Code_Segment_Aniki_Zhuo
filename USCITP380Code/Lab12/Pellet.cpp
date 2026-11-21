#include "Pellet.h"
#include "EnergyCatcher.h"
#include "EnergyGlass.h"
#include "EnergyCube.h"
#include "HealthComponent.h"

Pellet::Pellet(Game* game)
: Actor(game)
, mVelocity(0.0f, 0.0f, 0.0f)
, mLifeTime(0.0f)
, mPostTeleportTime(0.0f)
, mIsTeleport(false)
, mGreen(false)
{
	Mesh* mMesh = GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Sphere.gpmesh");

	mMeshComponent = new MeshComponent(this);
	mMeshComponent->SetMesh(mMesh);
	mMeshComponent->SetTextureIndex(1);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(25.0f, 25.0f, 25.0f);
}

void Pellet::SetVelocity(const Vector3& v)
{
	mVelocity = v;
}

void Pellet::OnUpdate(float deltaTime)
{
	mLifeTime += deltaTime;

	if (mIsTeleport)
	{
		mPostTeleportTime += deltaTime;
	}

	Vector3 pos = GetPosition();
	pos += mVelocity * deltaTime;
	SetPosition(pos);
	CalcWorldTransform();

	Player* player = GetGame()->GetPlayer();
	if (mCollisionComponent->Intersect(player->GetCollisionComponent()))
	{
		HealthComponent* playerHealth = player->GetComponent<HealthComponent>();
		if (playerHealth && !playerHealth->IsDead())
		{
			playerHealth->TakeDamage(100.0f, GetPosition());
		}

		SetState(ActorState::Destroy);
		return;
	}

	if (Catcher())
	{
		return;
	}

	if (Turret())
	{
		return;
	}

	if (mPostTeleportTime <= 0.25f)
	{
		if (mIsTeleport || Teleport())
		{
			return;
		}

		if (mLifeTime >= 0.25f)
		{
			for (auto collider : GetGame()->GetColliders())
			{
				if (mCollisionComponent->Intersect(collider->GetComponent<CollisionComponent>()))
				{
					EnergyCube* cube = nullptr;
					cube = dynamic_cast<EnergyCube*>(collider);
					if (cube)
					{
						mMeshComponent->SetTextureIndex(2);
						mGreen = true;
						return;
					}

					EnergyGlass* glass = nullptr;
					glass = dynamic_cast<EnergyGlass*>(collider);
					if (glass && mGreen)
					{
						return;
					}

					SetState(ActorState::Destroy);
					GetGame()->GetAudio()->PlaySound("PelletDeath.ogg", false, this, false);
					return;
				}
			}
		}
	}
}

bool Pellet::Teleport()
{
	Portal* blue = GetGame()->GetBluePortal();
	Portal* orange = GetGame()->GetOrangePortal();

	if (!blue || !orange)
	{
		return false;
	}

	if (blue->GetCollisionComponent()->Intersect(mCollisionComponent))
	{
		Vector3 newPos = blue->GetPortalOutVector(GetPosition(), orange, 1.0f);
		SetPosition(newPos);

		Vector3 newVel = blue->GetPortalOutVector(mVelocity, orange, 0.0f);
		newVel.Normalize();
		newVel *= 500.0f;
		SetVelocity(newVel);

		mIsTeleport = true;
		return true;
	}
	else if (orange->GetCollisionComponent()->Intersect(mCollisionComponent))
	{
		Vector3 newPos = orange->GetPortalOutVector(GetPosition(), blue, 1.0f);
		SetPosition(newPos);

		Vector3 newVel = orange->GetPortalOutVector(mVelocity, blue, 0.0f);
		newVel.Normalize();
		newVel *= 500.0f;
		SetVelocity(newVel);

		mIsTeleport = true;
		return true;
	}

	return false;
}

bool Pellet::Catcher()
{
	for (auto collider : GetGame()->GetColliders())
	{
		if (mCollisionComponent->Intersect(collider->GetComponent<CollisionComponent>()))
		{
			EnergyCatcher* catcher = dynamic_cast<EnergyCatcher*>(collider);
			if (catcher && !catcher->GetActive())
			{
				Vector3 pelletPos = catcher->GetPosition() + catcher->GetForward() * 40.0f;
				SetVelocity(Vector3::Zero);
				SetPosition(pelletPos);

				catcher->SetActive(true);
				GetGame()->GetAudio()->PlaySound("EnergyCaught.ogg", false, catcher);
				return true;
			}
		}
	}
	return false;
}

bool Pellet::Turret()
{
	for (auto collider : GetGame()->GetColliders())
	{
		if (mCollisionComponent->Intersect(collider->GetComponent<CollisionComponent>()))
		{
			HealthComponent* health = collider->GetComponent<HealthComponent>();
			if (health && !health->IsDead())
			{
				health->TakeDamage(100.0f, GetPosition());
				SetState(ActorState::Destroy);
				return true;
			}
		}
	}
	return false;
}