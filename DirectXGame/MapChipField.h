#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <map>
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kPlayer,
	kClear // クリアブロックを追加
};

struct MapChipDate {
	std::vector<std::vector<MapChipType>> date;
};

struct Block {
	float x, y, z;     // 3D座標
	bool isClearBlock; // クリアブロックかどうか
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	///< summary>
	/// 更新
	///  </summary>
	void Update();

	///< summary>
	/// 描画
	///  </summary>
	void Draw();

	void ResetMapChipDate();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(int xIndex, int zIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; };
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; };

	struct IndexSet {
		uint32_t xIndex;
		uint32_t zIndex;
	};

	IndexSet GetMapChipIndexSetByPosition(Vector3 pos);

	// 範囲矩形
	struct Rect {
		float left;  // 左端
		float right; // 右端
		float back;  // 下端
		float front; // 上端
	};

	Rect GetRectByIndex(int xIndex, int zIndex);

	int GetMapWidth() const;

	int GetMapHeight() const;

	bool IsClearBlock(Vector3 playerPos);

private:
	// 一ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const float kBlockDepth = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 34;
	static inline const uint32_t kNumBlockHorizontal = 100;

	
	MapChipDate mapChipDate_;
};
