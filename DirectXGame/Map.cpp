#include "Map.h"
#include <fstream>
#include <sstream>

Map::Map(float scale, const std::string& filename) { LoadFromCSV(filename); }

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

	// マップの幅と高さを設定
	height_ = static_cast<uint32_t>(mapData_.size());
	width_ = height_ > 0 ? static_cast<uint32_t>(mapData_[0].size()) : 0;
