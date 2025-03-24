#pragma once

//#include "Audio.h"
//#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class Time {

public:
	void Initialize();
	void Update();
	void Draw();
	bool IsTimeOver() const; // 時間が終了したかを判定

	Time(float limit = 5.0f) : limitTime(limit) { startTime = std::chrono::steady_clock::now(); }

private:
	// ビュープロジェクション
	ViewProjection* viewProjection_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 3Dモデル
	Model* model_ = nullptr;

	float limitTime; // 制限時間（秒）
	std::chrono::steady_clock::time_point startTime;
};
