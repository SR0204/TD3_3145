#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

// GameSceneクラスの前方宣言
class GameScene;

/// <summary>
/// てき
/// </summary>

class Enemy {
public:
	~Enemy();

	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	void OnCollosion();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void Move();

	void takeDamage(int damage, bool isPlayerAttack, float defenseRate);


public:
	Vector3 GetWorldPosition();
	Vector3 GetSize() const { return {kWidth_, kHeight_, kDepth_}; }
	// 各フラグのGetter
	bool IsStop() const { return isStop_; }
	bool IsEngage() const { return isEngage_; }
	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;

	uint32_t textureHandle_ = 0;

	// GameScene
	GameScene* gameScene_ ;

	// 各フラグ
	bool isStop_ = false;   // 停止フラグ
	bool isEngage_ = false; // 接触フラグ
	bool isDead_ = false;   // 死亡フラグ

	// 移動ベクトル
	Vector3 MoveVector_ = {0, 0, 0};

	// 移動方向変更タイマー
	int moveChangeTimer_ = 60;

	// 体力
	int HP_ = 10;

	// メンバ変数（private に）
	float kWidth_ = 1.0f;
	float kHeight_ = 2.0f;
	float kDepth_ = 1.0f;
};
