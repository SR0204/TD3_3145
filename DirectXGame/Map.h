#pragma once

#include<vector>
#include<Vector3.h>
#include <string>
#include"MapChipType.h"

class Map {
public:
	Map(const std::string& filename);

	// マップの幅を取得
	uint32_t GetWidth() const { return width_; }

	// マップの高さを取得
	uint32_t GetHeight() const { return height_; }

	// 指定された位置のマップチップの種類を取得
	MapChipType GetMapChipTypeByIndex(uint32_t x, uint32_t y) const;

	// 指定された位置のワールド座標を取得
	Vector3 GetMapChipPositionByIndex(uint32_t x, uint32_t y) const;

private:
	std::vector<std::vector<MapChipType>> mapData_;
	uint32_t width_ = 0;  // マップの幅
	uint32_t height_ = 0; // マップの高さ

	void LoadFromCSV(const std::string& filename);
};
