#include "Enemy.h"

#include "GameScene.h"
#include <cassert>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

Enemy::~Enemy() {}

void Enemy::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Enemy::Update() {
	if (HP_ <= 0) {
		isDead_ = true;
	}

	if (isDead_) {
		return;
	}

	// 一定時間ごとに移動方向を変更
	moveChangeTimer_--;
	if (moveChangeTimer_ <= 0) {
		Move();
	}

	// 現在の方向に移動
	worldTransform_.translation_.x += MoveVector_.x;
	/*worldTransform_.translation_.y += MoveVector_.y;*/
	worldTransform_.translation_.z += MoveVector_.z;

	// 壁にあたったら移動を止める
	if (isStop_) {
		worldTransform_.translation_.x = worldTransform_.translation_.x;
		worldTransform_.translation_.z = worldTransform_.translation_.z;
	}

#ifdef _DEBUG

	ImGui::Text("Enemy Position: %f, %f, %f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

#endif // _DEBUG

	// 更新行列
	worldTransform_.UpdateMatrixEnemy();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }

void Enemy::OnCollosion() { isStop_ = true; }

void Enemy::Move() {
	MoveVector_.x = (rand() % 100 - 50) / 500.0f; // -0.1 ～ 0.1
	//MoveVector_.y = (rand() % 100 - 50) / 500.0f; // -0.1 ～ 0.1
	MoveVector_.z = (rand() % 100 - 50) / 500.0f; // -0.1 ～ 0.1

	moveChangeTimer_ = 120; // 60フレームごとに変更
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（X, Y, Zのワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
