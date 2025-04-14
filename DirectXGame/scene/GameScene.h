#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "OverHeadCamera.h"

#include "Enemy.h"
#include "PlayerCamera.h"
#include "SkySphere.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <sstream>

class Player;
class Timer;

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

	void GenerateBlocks();

	void GenerateClearBlocks();

	bool IsFinished() const { return isFinished; }

	bool IsClear() const { return isClear_; }

	// 敵発生
	void SpawnEnemy(Vector3 position);

	// 敵発生データの読み込み
	void LoadEnemyPopData();

	// 敵発生コマンドの更新
	void UpDateEnemyPopCommands();

	// 時間表示用の関数
	void DrawTimeUI();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 3Dモデルデータ
	Model* modelBlock_ = nullptr;
	Model* modelClearBlock_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipFiled_;

	// マップモデル
	Model* mapModel_ = nullptr;

	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 天球
	SkySphere* SkySphere_ = nullptr;

	// 天球モデルデータ
	Model* modelSkySphere_ = nullptr;

	// 要素数
	const uint32_t kNumBlockVirtical = mapChipFiled_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipFiled_->GetNumBlockHorizontal();

	// 一ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;  // 横
	static inline const float kBlockHeight = 2.0f; // 縦

	Player* player_ = nullptr;                 // プレイヤー
	PlayerCamera* playerCamera_ = nullptr;     // プレイヤーのカメラ
	OverHeadCamera* overHeadCamera_ = nullptr; // 俯瞰カメラ

	bool isOverHeadCameraActive_; // 俯瞰カメラのアクティブ

	// 終了フラグ
	bool isFinished = false;

	bool isClear_ = false;

	// 音
	uint32_t music;

	// 音声再生
	uint32_t playMusic;

	// enemy
	Model* model_ = nullptr;
	uint32_t enemyTextureHandle_ = 0;
	Enemy* enemy_ = nullptr;
	std::list<Enemy*> enemies_;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;
	// 待機中フラグ
	bool waitFlag = false;
	// 待機タイマー
	int32_t waitTimer = 0;

	// 制限時間
	Timer* timer_ = nullptr;
	int numberTextures_[10]; //制限時間 0～9のテクスチャハンドル
};
