#include "LaserComponent.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

LaserComponent::LaserComponent(class Actor* owner)
: MeshComponent(owner)
, mIgnoreActor(nullptr)
, mLastHitActor(nullptr)
, mIsAble(true)
{
	SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime)
{
	mSegment.clear();

	if (!mIsAble)
	{
		return;
	}

	Vector3 start = mOwner->GetWorldPosition();
	Vector3 forward = mOwner->GetWorldForward();
	Vector3 firstEnd = start + forward * 350.0f;
	LineSegment firstSeg(start, firstEnd);

	CastInfo info;
	bool hit = SegmentCast(GetGame()->GetActors(), firstSeg, info, mIgnoreActor);
	if (hit)
	{
		firstSeg.mEnd = info.mPoint;
		mLastHitActor = info.mActor;
	}
	else
	{
		mLastHitActor = nullptr;
	}

	mSegment.emplace_back(firstSeg);

	if (hit)
	{
		Portal* enter = dynamic_cast<Portal*>(info.mActor);
		if (enter)
		{
			Game* game = GetGame();
			Portal* blue = game->GetBluePortal();
			Portal* orange = game->GetOrangePortal();
			if (blue && orange)
			{
				Portal* exit = nullptr;
				if (enter == blue)
				{
					exit = orange;
				}
				else
				{
					exit = blue;
				}

				Vector3 outDir = enter->GetPortalOutVector(forward, exit, 0.0f);
				outDir.Normalize();

				Vector3 newStart = enter->GetPortalOutVector(info.mPoint, exit, 1.0f);
				newStart += outDir * 5.5f;

				Vector3 newEnd = newStart + outDir * 350.0f;
				LineSegment secondSeg(newStart, newEnd);

				CastInfo info2;
				bool hit2 = SegmentCast(GetGame()->GetActors(), secondSeg, info2, exit);
				if (hit2)
				{
					secondSeg.mEnd = info2.mPoint;
					mLastHitActor = info2.mActor;
				}
				else
				{
					mLastHitActor = nullptr;
				}

				mSegment.emplace_back(secondSeg);
			}
		}
	}

	if (dynamic_cast<Portal*>(mLastHitActor))
	{
		mLastHitActor = nullptr;
	}
}

void LaserComponent::Draw(class Shader* shader)
{
	if (mMesh)
	{
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}

		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();

		for (const LineSegment& segment : mSegment)
		{
			Matrix4 transform = ComputeLaserTransform(segment);
			shader->SetMatrixUniform("uWorldTransform", transform);
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

Matrix4 LaserComponent::ComputeLaserTransform(const LineSegment& segment)
{
	Vector3 direction = segment.mEnd - segment.mStart;
	float length = direction.Length();
	direction.Normalize();

	Vector3 defaultForward = Vector3::UnitX;

	float dot = Vector3::Dot(defaultForward, direction);
	Quaternion rotation;

	if (Math::NearlyZero(dot - 1.0f))
	{
		rotation = Quaternion::Identity;
	}
	else if (Math::NearlyZero(dot + 1.0f))
	{
		rotation = Quaternion(Vector3::UnitY, Math::Pi);
	}
	else
	{
		Vector3 axis = Vector3::Cross(defaultForward, direction);
		axis.Normalize();
		float angle = Math::Acos(dot);
		rotation = Quaternion(axis, angle);
	}

	Matrix4 scaleMat = Matrix4::CreateScale(length, 1.0f, 1.0f);
	Matrix4 rotMat = Matrix4::CreateFromQuaternion(rotation);
	Matrix4 transMat = Matrix4::CreateTranslation(segment.PointOnSegment(0.5f));

	return scaleMat * rotMat * transMat;
}