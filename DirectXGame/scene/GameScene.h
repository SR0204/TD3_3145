#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "OverHeadCamera.h"

#include "PlayerCamera.h"
#include "SkySphere.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "character.h"
#include "game_manager.h"
#include "gauge.h"
#include <sstream>

class Player;
class Timer;
class Enemy;
class BattleScene;

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

	bool IsClear() const { return isClear_; }

	void SetClear();

	bool ShouldStartBattle() const { return shouldStartBattle_; }
	void ResetBattleTrigger() { shouldStartBattle_ = false; }

	// 時間表示用の関数
	void DrawTimeUI();

	void SetClearFlag(bool flag) { isClear_ = flag; }
	void SetFinishFlag(bool flag) { isFinished_ = flag; }

	bool IsBattleRequested() const;
	bool IsFinished() const;

	void ClearBattleRequest();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	GameManager gameManager;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 3Dモデルデータ
	Model* modelBlock_ = nullptr;

	// マップモデル
	Model* mapClearModel_ = nullptr;
	// マップチップフィールド
	MapChipField* mapChipFiled_;

	std::vector<std::vector<WorldTransform*>> worldTransformBlockList_;
	std::vector<std::vector<WorldTransform*>> worldTransformClearBlockList_;

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

	bool isClear_ = false;

	// 音
	uint32_t music;

	// 音声再生
	uint32_t playMusic;

	// enemy
	Model* model_ = nullptr;
	uint32_t enemyTextureHandle_ = 0;
	Enemy* enemy_ = nullptr;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;
	// 待機中フラグ
	bool waitFlag = false;
	// 待機タイマー
	int32_t waitTimer = 0;

	// 制限時間
	Timer* timer_ = nullptr;
	int numberTextures_[10]; // 制限時間 0～9のテクスチャハンドル

	bool isBattleTriggered_ = false; // 敵と接触したら true

	bool isDefending = false;       // 防御中かどうか
	int enemyAttackTimer = 0;       // 敵の攻撃タイマー
	Sprite* gaugeSprite_ = nullptr; // ゲージ表示用スプライト

	Gauge attackGauge;  // 攻撃用ゲージ
	Gauge defenseGauge; // 防御用ゲージ

	Character player;                    // プレイヤーキャラクター
	Character enemy;                     // 敵キャラクター
	Sprite* BackgroundSprite_ = nullptr; // 背景スプライト
	                                     // Sprite* EnemySprite_ = nullptr;      // 敵スプライト
	bool shouldStartBattle_;
	bool requestBattle_ = false;
	bool isFinished_ = false;

	//バトルシーン
	BattleScene* battleScene_ = nullptr;
	bool hasBattled_ = false; // もう戦ったかどうか
};
