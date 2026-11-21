#pragma once
#include <vector>
#include <string>
#include "Math.h"

class HeightMap
{
private:
	std::vector<std::vector<int>> mGrid;

public:
	HeightMap(const std::string& filePath);
	bool IsCellOnTrack(int row, int col) const;
	float GetHeightFromCell(int row, int col) const;
	Vector3 CellToWorld(int row, int col) const;
	Vector2 WorldToCell(const Vector2& pos) const;
	bool IsOnTrack(const Vector2& pos) const;
	float GetHeight(const Vector2& pos) const;

	const float CELL_SIZE = 40.55f;
	const float GRID_TOP = 1280.0f;
	const float GRID_LEFT = -1641.0f;
};
