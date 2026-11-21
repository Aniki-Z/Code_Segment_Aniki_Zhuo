#include "Portal.h"
#include "Game.h"
#include "PortalMeshComponent.h"
#include "Renderer.h"

Portal::Portal(class Game* game, bool isBlue)
: Actor(game)
, mIsBlue(isBlue)
{
	PortalMeshComponent* mPortalMeshComponent = new PortalMeshComponent(this);

	MeshComponent* mMeshComponent = new MeshComponent(this, true);
	mMeshComponent->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Portal.gpmesh"));

	if (isBlue)
	{
		mPortalMeshComponent->SetTextureIndex(0);
		mMeshComponent->SetTextureIndex(2);
	}
	else
	{
		mPortalMeshComponent->SetTextureIndex(1);
		mMeshComponent->SetTextureIndex(3);
	}

	mCollisionComponent = new CollisionComponent(this);

	Update(0.0f);
}

Vector3 Portal::GetPortalOutVector(const Vector3& initialVector, Portal* exitPortal, float w)
{
	Matrix4 inverseMatrix = this->GetWorldTransform();
	inverseMatrix.Invert();

	Vector3 localVector = Vector3::Transform(initialVector, inverseMatrix, w);

	Matrix4 rotationMatrix = Matrix4::CreateRotationZ(Math::Pi);

	Vector3 rotatedVector = Vector3::Transform(localVector, rotationMatrix, w);

	Matrix4 exitTransform = exitPortal->GetWorldTransform();
	Vector3 worldVector = Vector3::Transform(rotatedVector, exitTransform, w);

	return worldVector;
}

void Portal::CalcViewMatrix(struct PortalData& portalData, Portal* exitPortal)
{
	if (!exitPortal)
	{
		portalData.mView = Matrix4::CreateScale(0.0f);
		return;
	}

	Vector3 playerPos = GetGame()->GetPlayer()->GetPosition();
	Vector3 cameraForward = GetGame()->GetPlayer()->GetCameraComponent()->GetCameraForward();

	Vector3 portalCamPos = GetPortalOutVector(playerPos, exitPortal, 1.0f);

	Vector3 portalCamForward = GetPortalOutVector(cameraForward, exitPortal, 0.0f);

	Vector3 portalCamUp = exitPortal->GetWorldTransform().GetZAxis();

	Vector3 targetPos = portalCamPos + portalCamForward;

	portalData.mView = Matrix4::CreateLookAt(portalCamPos, targetPos, portalCamUp);

	portalData.mCameraPos = portalCamPos;
	portalData.mCameraForward = portalCamForward;
	portalData.mCameraUp = portalCamUp;
}

void Portal::OnUpdate(float deltaTime)
{
	if (mIsBlue)
	{
		CalcViewMatrix(GetGame()->GetRenderer()->GetBluePortal(), GetGame()->GetOrangePortal());
	}
	else
	{
		CalcViewMatrix(GetGame()->GetRenderer()->GetOrangePortal(), GetGame()->GetBluePortal());
	}
}