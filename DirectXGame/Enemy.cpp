#define NOMINMAX
#include "Enemy.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#define _USE_MATH_DEFINES
#include <MapChipField.h>
#include <math.h>

Enemy::~Enemy() { delete model_; }

void Enemy::Initialize(const Vector3 position) {
	worldTransform_.Initialize();                                         // ワールド変換データの初期化
	model_ = Model::CreateFromOBJ("Ghost");                               // モデルの生成
	textureHandle_ = TextureManager::Load("./Resources/Ghost/Ghost.png"); // テクスチャの読み込み

	worldTransform_.translation_ = position; // 初期配置

	chaseTimer_ = 0.0f;

	// 行き先ポイントの設定
	pointA_ = position;
	pointB_ = {position.x, position.y, position.z + 6.0f};
	targetPoint_ = pointB_;
}

void Enemy::Update() {
	const float deltaTime = 1.0f / 60.0f;
	Vector3& pos = worldTransform_.translation_;

	// 距離計算
	float dx = playerPosition_.x - pos.x;
	float dz = playerPosition_.z - pos.z;
	float distanceToPlayer = std::sqrt(dx * dx + dz * dz);

	if (distanceToPlayer < sightRange_ && CanSeePlayer()) {
		isChasing_ = true;
		chaseTimer_ = 0.0f; // 見えたら追跡開始＆リセット
	}

	// 追跡中の処理
	if (isChasing_) {
		// プレイヤー方向へ移動
		Vector3 dir = {dx, 0, dz};
		float length = std::sqrt(dir.x * dir.x + dir.z * dir.z);
		if (length != 0.0f) {
			dir.x /= length;
			dir.z /= length;
		}

		worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

		float chaseSpeed = 0.08f;
		Vector3 nextPos = {pos.x + dir.x * chaseSpeed, pos.y, pos.z + dir.z * chaseSpeed};

		// その位置にブロックがあるか確認
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(nextPos);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.zIndex) != MapChipType::kBlock) {
			// ブロックがなければ移動
			pos = nextPos;
		} else {
			// ブロックがあるなら追跡を中断してもよい
			isChasing_ = false;
		}

		chaseTimer_ += deltaTime;
		if (chaseTimer_ > maxChaseTime_) {
			isChasing_ = false; // 一定時間経ったら追跡終了
		}
	}

	// 追跡していない場合は、元の移動パターンに戻る
	else {
		// 方向ベクトルを求める
		Vector3 direction = {targetPoint_.x - pos.x, targetPoint_.y - pos.y, targetPoint_.z - pos.z};

		float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

		// 単位ベクトルに変換
		if (distance != 0.0f) {
			direction.x /= distance;
			direction.y /= distance;
			direction.z /= distance;
		}

		// 移動
		if (distance > moveSpeed_) {
			// 移動している方向を向く
			worldTransform_.rotation_.y = std::atan2(direction.x, direction.z);
			pos.x += direction.x * moveSpeed_;
			pos.y += direction.y * moveSpeed_;
			pos.z += direction.z * moveSpeed_;
		} else {
			// 到達したら行き先を切り替え
			if (targetPoint_.x == pointA_.x && targetPoint_.z == pointA_.z) {
				targetPoint_ = pointB_;
			} else {
				targetPoint_ = pointA_;
			}
		}
	}

	if (CheckCollisionWithPlayer()) {
		requestBattle_ = true; // バトル開始をリクエスト

		return; // これ以降の処理は不要になる
	}

	worldTransform_.UpdateMatrix(true);
}

void Enemy::Draw(ViewProjection* viewProjection) { model_->Draw(worldTransform_, *viewProjection, textureHandle_); }

void Enemy::SetPlayerPosition(const Vector3& playerPos) { playerPosition_ = playerPos; }

bool Enemy::CanSeePlayer() {
	if (!mapChipField_)
		return false;

	Vector3 start = worldTransform_.translation_;
	Vector3 end = playerPosition_;

	auto indexStart = mapChipField_->GetMapChipIndexSetByPosition(start);
	auto indexEnd = mapChipField_->GetMapChipIndexSetByPosition(end);

	int x0 = static_cast<int>(indexStart.xIndex);
	int z0 = static_cast<int>(indexStart.zIndex);
	int x1 = static_cast<int>(indexEnd.xIndex);
	int z1 = static_cast<int>(indexEnd.zIndex);

	int dx = std::abs(x1 - x0);
	int dz = std::abs(z1 - z0);
	int sx = (x0 < x1) ? 1 : -1;
	int sz = (z0 < z1) ? 1 : -1;

	int err = dx - dz;

	while (true) {
		if (mapChipField_->GetMapChipTypeByIndex(x0, z0) == MapChipType::kBlock) {
			return false; // ブロックに視線が遮られた
		}

		if (x0 == x1 && z0 == z1)
			break;

		int e2 = 2 * err;
		if (e2 > -dz) {
			err -= dz;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			z0 += sz;
		}
	}

	return true;
}

bool Enemy::CheckCollisionWithPlayer() {
	Vector3 pos = worldTransform_.translation_;
	float dx = playerPosition_.x - pos.x;
	float dz = playerPosition_.z - pos.z;
	float distance = std::sqrt(dx * dx + dz * dz);

	float collisionRadius = 1.0f; // プレイヤーと敵の当たり判定の合計半径（調整してね）
	return distance < collisionRadius;
}
