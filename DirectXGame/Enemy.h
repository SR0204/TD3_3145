#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include<cmath>

class MapChipField;

class Enemy {
public:
	~Enemy();

	void Initialize(const Vector3 position, Model* model);

	void Update();

	void Draw(ViewProjection* viewProjection);

	void SetTranslation(Vector3 translation) { worldTransform_.translation_ = translation; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void SetPlayerPosition(const Vector3& position) { playerPosition_ = position; }

private:
	bool CanSeePlayer();

	bool CheckCollisionWithPlayer(const Vector3& playerPos, float collisionRadius);

private:
	// ワールド変形
	WorldTransform worldTransform_;
	// モデルデータ
	Model* model_ = nullptr;

	uint32_t textureHandle_ = 0;

	MapChipField* mapChipField_;

	Vector3 pointA_;      // 移動開始位置
	Vector3 pointB_;      // 移動終了位置
	Vector3 targetPoint_; // 今向かっている目標地点

	float elapsedTime_ = 0.0f;  // 経過時間
	float moveInterval_ = 3.0f; // 移動間隔（秒）
	float moveSpeed_ = 0.05f;   // 1フレームごとの移動速度

	Vector3 playerPosition_;    // プレイヤーの位置（外から渡す）
	bool isChasing_ = false;    // 追跡中フラグ
	float chaseTimer_ = 0.0f;   // 追跡時間カウント
	float maxChaseTime_ = 1.0f; // 追跡最大秒数
	float sightRange_ = 10.0f;   // 視界範囲（距離）
};
