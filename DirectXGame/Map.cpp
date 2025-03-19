#include "Map.h"
#include <fstream>
#include <sstream>

Map::Map(const std::string& filename) { LoadFromCSV(filename); }

//**マップチップの種類を取得**
MapChipType Map::GetMapChipTypeByIndex(uint32_t x, uint32_t y) const {
	if (x >= width_ || y >= height_) {
		return MapChipType::kBlank; // 範囲外なら空白
	}
	return mapData_[y][x];
}

//**マップチップのワールド座標を取得**
Vector3 Map::GetMapChipPositionByIndex(uint32_t x, uint32_t y) const { return Vector3(static_cast<float>(x), 0.0f, static_cast<float>(y)); }

// CSVファイルを読み込んで `mapData_` を作成
void Map::LoadFromCSV(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		printf("Failed to open map file: %s\n", filename.c_str());
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<MapChipType> row;
		std::stringstream ss(line);
		std::string cell;
		while (std::getline(ss, cell, ',')) {
			row.push_back(static_cast<MapChipType>(std::stoi(cell)));
		}
		mapData_.push_back(row);
	}

	file.close();

	// 幅と高さを設定
	if (!mapData_.empty()) {
		height_ = static_cast<uint32_t>(mapData_.size());   // 行数が高さ
		width_ = static_cast<uint32_t>(mapData_[0].size()); // 最初の行の要素数が幅
	} else {
		height_ = 0;
		width_ = 0;
	}
}
