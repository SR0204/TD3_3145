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
class Clear {
public: // 引数を書くところ
	/// <summary>
	/// 初期化
	/// </summary>
	// 初期化
	Clear();
	~Clear();

	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

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