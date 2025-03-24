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

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {

	IndexSet indexSet = {};

	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);

	indexSet.zIndex = static_cast<uint32_t>(position.z / kBlockHeight);

	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t zIndex) {
	// 指定ブロックの中心座標を取得する
	Vector3 center = GetMapChipPositionByIndex(xIndex, zIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.back = center.z - kBlockHeight / 2.0f;
	rect.front = center.z + kBlockHeight / 2.0f;
	
	return rect;
}
int MapChipField::GetMapWidth() const {
	return static_cast<int>(mapChipDate_.date[0].size()); // 1行の要素数がマップの横幅
}

int MapChipField::GetMapHeight() const {
	return static_cast<int>(mapChipDate_.date.size()); // 行数がマップの縦幅
}
