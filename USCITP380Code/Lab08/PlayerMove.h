#pragma once
#include "VehicleMove.h"
#include "Math.h"

class PlayerMove : public VehicleMove
{
private:
	class HeightMap* mHeightMap;

public:
	PlayerMove(class Actor* owner);

	void Update(float deltaTime) override;
	void ProcessInput(const Uint8* keyState) override;

	void OnLapChange(int newLap) override;

	const float MOVE_SPEED = 300.0f;
	const int SECOND_LAP = 2;
	const int THIRD_LAP = 3;
};
