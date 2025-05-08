#pragma once
#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SkySphere.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>

class BattleScene {

public:
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const;

private:
	// 終了フラグ
	bool finished_ = false;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	DirectXCommon* dxCommon_ = nullptr;

	float timer_ = 0.0f;
	static inline const float kMotionTime = 1.0f;

	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	uint32_t textureHandle_ = 0;
	Sprite* sprite;

	// 音
	uint32_t music;

	// 音声再生
	uint32_t playMusic;
};
