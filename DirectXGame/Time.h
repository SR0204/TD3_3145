//#include "imgui.h"
#include "Audio.h"
#include <DirectXCommon.cpp>
#include <Input.h>
#include <Model.h>
#include <chrono>
#include <iostream>
#include <sstream>

#pragma once
class Time {
public:
	void Initialize();
	void Update();
	void Draw();
	bool IsTimeOver() const; // 時間が終了したかを判定

	Time(float limit = 5.0f) : limitTime(limit) { startTime = std::chrono::steady_clock::now(); }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	//Audio* audio_ = nullptr;

	// ビュープロジェクション
	ViewProjection* viewProjection_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 3Dモデル
	Model* model_ = nullptr;

	float limitTime; // 制限時間（秒）
	std::chrono::steady_clock::time_point startTime;
};
