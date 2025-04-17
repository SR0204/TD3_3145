
#include "Player.h"
#include <algorithm>
#include "MathUtilityForText.h"
#include "TextureManager.h"

#include "GameScene.h"
#include "MapChipField.h"

#include <cassert>
#include <iostream>
#include <math.h>

Player::~Player() { delete model_; }

void Player::Initialize(MapChipField* mapChipField, GameScene* gameScene) {
	// ワールド変換初期化
	worldTransform_.Initialize();

	// 外部オブジェクトを紐付け
	mapChipField_ = mapChipField;
	gameScene_ = gameScene;

	// スタート座標をマップから取得
	Vector3 startPos = mapChipField_->GetMapChipPositionByIndex(2, 2);
	worldTransform_.translation_ = startPos;
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// プレイヤー移動速度初期化
	moveVel_ = {0.0f, 0.0f, 0.0f};
	rotateVel_ = {0.02f, 0.02f, 0.0f}; // 回転スピード設定

	// 衝突判定用パラメータ（幅・奥行きなど）
	// 例：箱サイズに合わせる
	kWidth = 1.8f; // プレイヤーの横幅（ブロック単位）
	kDepth = 1.8f; // プレイヤーの奥行き（ブロック単位）
	 
	// 入力クラス取得
	input_ = Input::GetInstance();

	// モデルやテクスチャ初期化（必要なら）
	model_ = Model::Create();
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// クリア状態リセット
	isClear_ = false;
}

void Player::Update() {
	if (isClear_) {
		moveVel_ = {0.0f, 0.0f, 0.0f}; // クリア後は停止
		return;
	}

	HandleInput();      // キーやスティックの入力取得
	ApplyMovement();    // 移動計算と適用
	ProcessCollision(); // 衝突判定
	ProcessGameClear(); // ゴール判定
}

void Player::HandleInput() {
	moveVel_ = {0.0f, 0.0f, 0.0f};
	bool moved = false;

	if (input_->PushKey(DIK_W)) {
		moveVel_.z += 0.1f;
		moved = true;
	}
	if (input_->PushKey(DIK_S)) {
		moveVel_.z -= 0.1f;
		moved = true;
	}
	if (input_->PushKey(DIK_A)) {
		moveVel_.x -= 0.1f;
		moved = true;
	}
	if (input_->PushKey(DIK_D)) {
		moveVel_.x += 0.1f;
		moved = true;
	}

	if (input_->PushKey(DIK_K))
		worldTransform_.rotation_.x += rotateVel_.x;
	if (input_->PushKey(DIK_I))
		worldTransform_.rotation_.x -= rotateVel_.x;
	if (input_->PushKey(DIK_L))
		worldTransform_.rotation_.y += rotateVel_.y;
	if (input_->PushKey(DIK_J))
		worldTransform_.rotation_.y -= rotateVel_.y;

	// clampの代わり
	if (worldTransform_.rotation_.x < -pi / 4.0f) {
		worldTransform_.rotation_.x = -pi / 4.0f;
	}
	if (worldTransform_.rotation_.x > pi / 4.0f) {
		worldTransform_.rotation_.x = pi / 4.0f;
	}
}

void Player::ApplyMovement() {
	Vector3 proposedMove = moveVel_;

	// 衝突判定で移動ベクトル修正
	if (!DetectCollision(proposedMove)) {
		worldTransform_.translation_ += proposedMove;
	} else {
		moveVel_ = {0.0f, 0.0f, 0.0f}; // 衝突したら停止
	}
}

void Player::ProcessCollision() {
	// 衝突判定中にクリアブロックが含まれていたらProcessGameClear内で処理
}

void Player::ProcessGameClear() {
	int mapX = static_cast<int>(worldTransform_.translation_.x / kBlockSize);
	int mapZ = static_cast<int>(worldTransform_.translation_.z / kBlockSize);

	if (mapX < 0 || mapZ < 0 || mapX >= mapChipField_->GetMapWidth() || mapZ >= mapChipField_->GetMapHeight()) {
		return;
	}

	MapChipType type = mapChipField_->GetMapChipTypeByIndex(mapX, mapZ);
	if (type == MapChipType::kClear && !isClear_) {
		std::cout << "ゴール！プレイヤーリセット！" << std::endl;
		isClear_ = true;
		OnGameClear();
	}
}

bool Player::DetectCollision(Vector3& outMove) {
	bool hit = false;
	std::array<Vector3, 4> checkPoints = {
	    worldTransform_.translation_ + outMove + Vector3(-kWidth / 2, 0, -kDepth / 2),
	    worldTransform_.translation_ + outMove + Vector3(+kWidth / 2, 0, -kDepth / 2),
	    worldTransform_.translation_ + outMove + Vector3(-kWidth / 2, 0, +kDepth / 2),
	    worldTransform_.translation_ + outMove + Vector3(+kWidth / 2, 0, +kDepth / 2),
	};

	for (const auto& point : checkPoints) {
		int mapX = static_cast<int>(std::floor(point.x / kBlockSize));
		int mapZ = static_cast<int>(std::floor(point.z / kBlockSize));

		if (mapX < 0 || mapZ < 0 || mapX >= mapChipField_->GetMapWidth() || mapZ >= mapChipField_->GetMapHeight()) {
			hit = true;
			continue;
		}

		MapChipType type = mapChipField_->GetMapChipTypeByIndex(mapX, mapZ);
		if (type == MapChipType::kBlock) {
			hit = true;
			break;
		}

		// ゴール判定は移動中にも拾える
		if (type == MapChipType::kClear && !isClear_) {
			isClear_ = true;
			OnGameClear();
			break;
		}
	}

	if (hit) {
		outMove = {0.0f, 0.0f, 0.0f}; // 衝突時は移動キャンセル
	}

	return hit;
}

void Player::OnGameClear() {
	std::cout << "OnGameClear: isClear_ = " << isClear_ << std::endl;
	if (!isClear_) {
		isClear_ = true;
		gameScene_->IsClear(); // ステージクリア処理へ
	}
}

void Player::Draw(ViewProjection& viewProjection) { model_->Draw(worldTransform_, viewProjection, textureHandle_); }