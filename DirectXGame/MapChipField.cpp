#define NOMINMAX
#include "MapChipField.h"
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank },
    {"1", MapChipType::kBlock },
    {"4", MapChipType::kPlayer},
};
}

void MapChipField::Initialize() {}

void MapChipField::Update() {}

void MapChipField::Draw() {}

void MapChipField::ResetMapChipDate() {

	// マップチップデータをリセット
	mapChipDate_.date.clear();
	mapChipDate_.date.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDateLine : mapChipDate_.date) {
		mapChipDateLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	// マップチップデータをリセット
	ResetMapChipDate();

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;

	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	std::string line;

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		getline(mapChipCsv, line);

		// 一行分の文字列をストリームに変換して解析しやすくなる
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipDate_.date[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(int xIndex, int zIndex) {
	if (xIndex < 0 || xIndex >= static_cast<int>(kNumBlockHorizontal)) {
		return MapChipType::kBlank;
	}
	if (zIndex < 0 || zIndex >= static_cast<int>(kNumBlockVirtical)) {
		return MapChipType::kBlank;
	}
	return mapChipDate_.date[zIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex) { return Vector3(kBlockWidth * xIndex, 0, kBlockHeight * zIndex); }

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(Vector3 pos) {
	int x = static_cast<int>(std::floor(pos.x / kBlockWidth));
	int z = static_cast<int>(std::floor(pos.z / kBlockHeight));

	// 範囲外アクセス防止
	unsigned int xIndex = static_cast<unsigned int>(std::max(0, std::min(x, static_cast<int>(GetMapWidth() - 1))));
	unsigned int zIndex = static_cast<unsigned int>(std::max(0, std::min(z, static_cast<int>(GetMapHeight() - 1))));

	return {xIndex, zIndex};
}

MapChipField::Rect MapChipField::GetRectByIndex(int xIndex, int zIndex) {
	float left = xIndex * kBlockWidth;
	float right = left + kBlockWidth;
	float front = zIndex * kBlockHeight;
	float back = front + kBlockHeight;

	return {left, right, front, back};
}
int MapChipField::GetMapWidth() const {
	return static_cast<int>(mapChipDate_.date[0].size()); // 1行の要素数がマップの横幅
}

int MapChipField::GetMapHeight() const {
	return static_cast<int>(mapChipDate_.date.size()); // 行数がマップの縦幅
}
