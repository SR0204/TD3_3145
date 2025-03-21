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
};



struct MapChipDate {
	std::vector<std::vector<MapChipType>> date;
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

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; };
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; };

	
	

	struct IndexSet {
		uint32_t xIndex;
		uint32_t zIndex;
	};

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	// 範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float back; // 下端
		float front;  // 上端
	};

	Rect GetRectByIndex(uint32_t xIndex, uint32_t zIndex);	

private:
	// 一ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const float kBlockDepth = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipDate mapChipDate_;
};
