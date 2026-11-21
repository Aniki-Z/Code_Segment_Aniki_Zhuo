#include "Player.h"

Player::Player(Game* game)
: Actor(game)
{
	Actor::SetScale(1.0f);

	mPlayerMove = new PlayerMove(this);

	mCameraComponent = new CameraComponent(this);

	mCollisionComponent = new CollisionComponent(this);
	mCollisionComponent->SetSize(50, 100, 50);

	mHasGun = false;

	mHealthComponent = new HealthComponent(this);
	mHealthComponent->SetOnDeathCallback([this] {
		GetGame()->GetPlayer()->GetPlayerMove()->SetDead(true);
	});

	mHealthComponent->SetOnDamageCallback([this](const Vector3& hitLoc) {
		Vector3 dir = hitLoc - GetWorldPosition();
		dir.z = 0.0f;
		dir.Normalize();

		Vector3 forward = GetForward();
		forward.z = 0.0f;
		forward.Normalize();

		float dot = Math::Clamp(Vector3::Dot(forward, dir), -1.0f, 1.0f);
		float angle = Math::Acos(dot);

		Vector3 cross = Vector3::Cross(forward, dir);
		if (cross.z > 0.0f)
		{
			angle = -angle;
		}

		mHUD->PlayerTakeDamage(angle);
	});

	mHUD = new HUD(this);
}

void Player::HasGunNow()
{
	mHasGun = true;
	new PlayerMesh(GetGame());
}