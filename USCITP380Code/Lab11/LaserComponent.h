#pragma once
#include "MeshComponent.h"
#include "SegmentCast.h"
#include "Game.h"
#include <vector>

class LaserComponent : public MeshComponent
{
public:
	LaserComponent(class Actor* owner);

	void Update(float deltaTime) override;
	void Draw(class Shader* shader) override;
	Matrix4 ComputeLaserTransform(const LineSegment& segment);

	void SetIgnoreActor(Actor* actor) { mIgnoreActor = actor; }

	Actor* GetLastHitActor() { return mLastHitActor; }

	void SetIsAble(bool able) { mIsAble = able; }

private:
	std::vector<LineSegment> mSegment;

	Actor* mIgnoreActor;
	Actor* mLastHitActor;

	bool mIsAble;
};