#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "OverHeadCamera.h"
#include "Skydome.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	ViewProjection viewProjection_;//ビュープロジェクション
	Player* player_ = nullptr;// プレイヤー
	PlayerCamera* playerCamera_ = nullptr;//プレイヤーのカメラ
	OverHeadCamera* overHeadCamera_ = nullptr;//俯瞰カメラ
	Skydome* skydome_ = nullptr;//天球

	bool isOverHeadCameraActive_;//俯瞰カメラのアクティブ

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
