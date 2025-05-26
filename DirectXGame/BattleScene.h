#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Gauge.h"
#include "Input.h"
#include "Player.h"
#include "Sprite.h"
#include "character.h"
#include <cstdint>

class BattleScene {
public:
	// 勝敗結果の列挙型
	enum class BattleResult { PlayerWin, PlayerLose };

	// 初期化・終了処理
	void Initialize();
	void Finalize();

	// 毎フレーム更新・描画処理
	void Update();
	void Draw();

	// バトル終了チェックと結果取得
	bool IsFinished();
	BattleResult GetResult() const;

private:
	// 各種インスタンスへのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// 背景スプライトとテクスチャ
	Sprite* bgSprite_ = nullptr;
	uint32_t bgTex_ = 0;

	// プレイヤー・敵
	Character player;
	Character enemy;

	// 攻撃・防御ゲージ
	Gauge* attackGauge = nullptr;
	Gauge* defenseGauge = nullptr;

	// 防御中フラグ、攻撃タイマー
	bool isDefending = false;
	int enemyAttackTimer = 0;

	// シーン終了フラグ、勝敗結果
	bool isFinished_ = false;
	BattleResult result_ = BattleResult::PlayerWin; // 初期値
};
