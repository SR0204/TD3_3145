#define NOMINMAX
#include "Player.h"

#include "MathUtilityForText.h"
#include "TextureManager.h"
#define _USE_MATH_DEFINES
#include "GameScene.h"
#include "MapChipField.h"
#include <cassert>
#include <iostream>
#include <math.h>

Player::Player(GameScene* gameScene) {
	gameScene_ = gameScene; // gameScene_ に gameScene を代入
}

Player::~Player() { delete model_; }

void Player::Initialize(const Vector3& position) {
	worldTransform_.Initialize();                           // ワールド変換データの初期化
	model_ = Model::CreateFromOBJ("player");                // モデルの生成
	textureHandle_ = TextureManager::Load("uvChecker.png"); // テクスチャの読み込み
	input_ = Input::GetInstance();                          // 入力のインスタンスを取得

	vibrationVal_ = {};
	deadZone_ = 8000;     // デッドゾーン
	trigerDeadZone_ = 30; // トリガーのデッドゾーン

	leftStickLange_ = 0.0f;

	kWidth = 0.5f;
	kHeight = 0.5f;
	kDepth = 0.5f;
	rotateVel_ = {0.0f, 0.05f, 0.0f}; // 旋回速度
	moveVel_ = {0.1f, 0.0f, 0.1f};    // 移動速度

	worldTransform_.translation_ = position; // 初期配置

	isClear_ = false;
}

void Player::Update() {

	// コントローラー関係
	/// ーーーここからーーー

	DWORD dwResult = XInputGetState(0, &state_);                      // コントローラーの状態を取得
	vibration_.wLeftMotorSpeed = static_cast<WORD>(vibrationVal_.x);  // 左モーターの振動強さ（最大値）
	vibration_.wRightMotorSpeed = static_cast<WORD>(vibrationVal_.y); // 右モーターの振動強さ（最大値）
	XInputSetState(0, &vibration_);                                   // コントローラー0のバイブレーションを設定
	int leftThumbX = state_.Gamepad.sThumbLX;                         // 左スティックのX値
	int leftThumbY = state_.Gamepad.sThumbLY;                         // 左スティックのY値
	int rightThumbX = state_.Gamepad.sThumbRX;                        // 右スティックのX値
	int rightThumbY = state_.Gamepad.sThumbRY;                        // 右スティックのX値
	BYTE leftTrigger = state_.Gamepad.bLeftTrigger;                   // 左トリガーの値
	BYTE rightTrigger = state_.Gamepad.bRightTrigger;                 // 右トリガーの値

	// デッドゾーンの適用
	ApplyDeadZone(leftThumbX);
	ApplyDeadZone(leftThumbY);
	ApplyDeadZone(rightThumbX);
	ApplyDeadZone(rightThumbY);
	ApplyTriggerDeadZone(leftTrigger);
	ApplyTriggerDeadZone(rightTrigger);

	// 長さを求める
	leftStickLange_ = sqrtf(powf(static_cast<float>(leftThumbX), 2.0f) + powf(static_cast<float>(leftThumbY), 2.0f));
	rightStickLange_ = sqrtf(powf(static_cast<float>(rightThumbX), 2.0f) + powf(static_cast<float>(rightThumbY), 2.0f));

	// 単位ベクトルを求める
	leftStickUnitVector_ = {static_cast<float>(leftThumbX) / leftStickLange_, static_cast<float>(leftThumbY) / leftStickLange_};
	rightStickUnitVector_ = {static_cast<float>(rightThumbX) / rightStickLange_, static_cast<float>(rightThumbY) / rightStickLange_};

	//// コントローラーの状態をImGuiで出力
	
	if (dwResult == ERROR_SUCCESS) {
		
	}

	/// ーーーここまでーーー

	const float pi = float(M_PI);

	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(true);

	// 衝突判定用データ
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = moveVel_;
	collisionMapInfo.isBlocked = false;

	if (isClear_) {
		return;
	}

	moveVel_ = {0.1f, 0.0f, 0.1f};

	// 衝突判定
	if (CheckMapCollision(collisionMapInfo) || CheckCollisionWithCSVMap(collisionMapInfo)) {
		collisionMapInfo.isBlocked = true;
		moveVel_ = {0.0f, 0.0f, 0.0f};
	}

	// キーボード旋回
	bool rotated = false;
	if (input_->PushKey(DIK_K)) {
		worldTransform_.rotation_.x += rotateVel_.x;
		rotated = true;
	}
	if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.x -= rotateVel_.x;
		rotated = true;
	}
	if (input_->PushKey(DIK_L)) {
		worldTransform_.rotation_.y += rotateVel_.y;
		rotated = true;
	}
	if (input_->PushKey(DIK_J)) {
		worldTransform_.rotation_.y -= rotateVel_.y;
		rotated = true;
	}

	// 右スティックでカメラ回転
	if (fabs(rightStickUnitVector_.x) > 0.0f || fabs(rightStickUnitVector_.y) > 0.0f) {
		worldTransform_.rotation_.x += rightStickUnitVector_.y * -rotateVel_.x;
		worldTransform_.rotation_.y += rightStickUnitVector_.x * rotateVel_.y;
		rotated = true;
	}

	// 旋回後は再移動許可
	if (rotated) {
		collisionMapInfo.isBlocked = false;
		moveVel_ = {0.1f, 0.0f, 0.1f};
	}

	// 回転制限
	if (worldTransform_.rotation_.y >= pi || worldTransform_.rotation_.y <= -pi) {
		worldTransform_.rotation_.y *= -1;
	}
	if (worldTransform_.rotation_.x > pi / 4.0f) {
		worldTransform_.rotation_.x = pi / 4.0f;
	}
	if (worldTransform_.rotation_.x < -pi / 4.0f) {
		worldTransform_.rotation_.x = -pi / 4.0f;
	}

	// 入力から移動ベクトル計算
	Vector3 nextMove = {0.0f, 0.0f, 0.0f};

	if (input_->PushKey(DIK_W)) {
		nextMove.z += cosf(worldTransform_.rotation_.y) * moveVel_.z;
		nextMove.x += sinf(worldTransform_.rotation_.y) * moveVel_.x;
	}
	if (input_->PushKey(DIK_S)) {
		nextMove.z -= cosf(worldTransform_.rotation_.y) * moveVel_.z;
		nextMove.x -= sinf(worldTransform_.rotation_.y) * moveVel_.x;
	}
	if (input_->PushKey(DIK_D)) {
		nextMove.z += cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.z;
		nextMove.x += sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.x;
	}
	if (input_->PushKey(DIK_A)) {
		nextMove.z -= cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.z;
		nextMove.x -= sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.x;
	}

	// スティック移動も合成
	nextMove.z += cosf(worldTransform_.rotation_.y) * (leftThumbY / 32767.0f) * moveVel_.z;
	nextMove.x += sinf(worldTransform_.rotation_.y) * (leftThumbY / 32767.0f) * moveVel_.x;
	nextMove.z += cosf(worldTransform_.rotation_.y + pi / 2.0f) * (leftThumbX / 32767.0f) * moveVel_.z;
	nextMove.x += sinf(worldTransform_.rotation_.y + pi / 2.0f) * (leftThumbX / 32767.0f) * moveVel_.x;

	// 衝突確認
	collisionMapInfo.move = nextMove;
	if (!CheckMapCollision(collisionMapInfo) && !CheckCollisionWithCSVMap(collisionMapInfo)) {
		worldTransform_.translation_ += nextMove;
	} else {
		//moveVel_ = {0.0f, 0.0f, 0.0f};
	}

	// まずクリアブロック判定
	if (CheckCollisionWithClearBlock(collisionMapInfo)) {
		// クリアしたならもうこれ以上進めない
		return;
	}
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

bool Player::CheckMapCollision(CollisionMapInfo& info) {
	// クリアブロックとの衝突（最優先で判定）
	if (CheckCollisionWithClearBlock(info)) {
		return true;
	}

	// CSVマップのブロックとの衝突判定
	bool hitCSVBlock = CheckCollisionWithCSVMap(info);

	// 移動方向ごとの補正処理（詳細な接触調整）
	bool hitDirectional = CheckMapCollisionDirection(info, Vector3(kWidth, 0, 0)) ||  // 右
	                      CheckMapCollisionDirection(info, Vector3(-kWidth, 0, 0)) || // 左
	                      CheckMapCollisionDirection(info, Vector3(0, 0, kDepth)) ||  // 前
	                      CheckMapCollisionDirection(info, Vector3(0, 0, -kDepth));   // 後

	// いずれかで衝突したら true を返す
	return hitCSVBlock || hitDirectional;
}

bool Player::CheckMapCollisionDirection(CollisionMapInfo& info, const Vector3& direction) {
	// 移動方向と逆なら判定しない
	if (direction.x != 0 && (info.move.x <= 0 && direction.x > 0 || info.move.x >= 0 && direction.x < 0)) {
		return false;
	}
	if (direction.z != 0 && (info.move.z <= 0 && direction.z > 0 || info.move.z >= 0 && direction.z < 0)) {
		return false;
	}

	// 判定用の4点（足元の4隅）
	std::array<Vector3, 4> positionNew = {
	    worldTransform_.translation_ + info.move + direction + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
	    worldTransform_.translation_ + info.move + direction + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
	    worldTransform_.translation_ + info.move + direction + Vector3(-kWidth / 2.0f, 0, +kDepth / 2.0f),
	    worldTransform_.translation_ + info.move + direction + Vector3(+kWidth / 2.0f, 0, +kDepth / 2.0f),
	};

	bool hit = false;
	MapChipField::IndexSet indexSet;
	for (const auto& pos : positionNew) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.zIndex) == MapChipType::kBlock) {
			hit = true;
			break;
		}
	}

	if (hit) {
		// 今いる位置と direction から衝突しているマスの矩形を取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + direction);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.zIndex);

		if (direction.x > 0) {
			// 右方向への移動時
			float rightEdge = worldTransform_.translation_.x + kWidth / 2.0f + kBlank;
			info.move.x = std::min(info.move.x, rect.left - rightEdge);
		} else if (direction.x < 0) {
			// 左方向への移動時
			float leftEdge = worldTransform_.translation_.x - kWidth / 2.0f - kBlank;
			info.move.x = std::max(info.move.x, rect.right - leftEdge);
		}

		if (direction.z > 0) {
			// 前方向への移動時
			float frontEdge = worldTransform_.translation_.z + kDepth / 2.0f + kBlank;
			info.move.z = std::min(info.move.z, rect.front - frontEdge);
		} else if (direction.z < 0) {
			// 後方向への移動時
			float backEdge = worldTransform_.translation_.z - kDepth / 2.0f - kBlank;
			info.move.z = std::max(info.move.z, rect.back - backEdge);
		}

		info.hitWall = true;

		// デバッグ出力
		std::cout << "Collision " << direction.x << ", " << direction.z << "!" << std::endl;
		return true;
	}

	return false;
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	static const std::array<Vector3, kNumCorner> offsetTable = {
	    Vector3(+kWidth / 2.0f, -kHeight / 2.0f, +kDepth / 2.0f), // kRightBottomFront
	    Vector3(-kWidth / 2.0f, -kHeight / 2.0f, +kDepth / 2.0f), // kLeftBottomFront
	    Vector3(+kWidth / 2.0f, +kHeight / 2.0f, +kDepth / 2.0f), // kRightTopFront
	    Vector3(-kWidth / 2.0f, +kHeight / 2.0f, +kDepth / 2.0f), // kLeftTopFront
	    Vector3(+kWidth / 2.0f, -kHeight / 2.0f, -kDepth / 2.0f), // kRightBottomBack
	    Vector3(-kWidth / 2.0f, -kHeight / 2.0f, -kDepth / 2.0f), // kLeftBottomBack
	    Vector3(+kWidth / 2.0f, +kHeight / 2.0f, -kDepth / 2.0f), // kRightTopBack
	    Vector3(-kWidth / 2.0f, +kHeight / 2.0f, -kDepth / 2.0f)  // kLeftTopBack
	};

	assert(static_cast<size_t>(corner) < offsetTable.size());
	return center + offsetTable[static_cast<size_t>(corner)];
}

bool Player::CheckCollisionWithCSVMap(CollisionMapInfo& info) {
	// プレイヤーの足元4隅の座標を取得
	std::array<Vector3, 4> corners = {
	    worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
	    worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
	    worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, +kDepth / 2.0f),
	    worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, +kDepth / 2.0f),
	};

	bool hitX = false;
	bool hitZ = false;

	for (const auto& corner : corners) {
		int mapX = static_cast<int>(std::floor(corner.x / kBlockSize));
		int mapZ = static_cast<int>(std::floor(corner.z / kBlockSize));

		// マップ外なら衝突扱い
		if (mapX < 0 || mapZ < 0 || mapX >= mapChipField_->GetMapWidth() || mapZ >= mapChipField_->GetMapHeight()) {
			hitX = true;
			hitZ = true;
			continue;
		}

		MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(mapX, mapZ);

		// 壁との衝突判定
		if (chipType == MapChipType::kBlock) {
			if (corner.x != worldTransform_.translation_.x) {
				hitX = true;
			}
			if (corner.z != worldTransform_.translation_.z) {
				hitZ = true;
			}
		}

		// クリアブロックとの衝突（ゲームクリア処理）
		if (chipType == MapChipType::kClear) {
			std::cout << "ゲームクリア！" << std::endl;
			OnGameClear();
			return true;
		}
	}

	// X/Z方向ごとの衝突補正
	if (hitX) {
		info.move.x = 0;
	}
	if (hitZ) {
		info.move.z = 0;
	}

	info.hitWall = hitX || hitZ;
	return info.hitWall;
}

void Player::OnGameClear() {
	// ゲームクリアフラグを立てる
	isClear_ = true;

	// クリア画面に遷移する処理を追加（例：SceneManagerを使用）
	gameScene_->SetClearFlag(true);
	gameScene_->SetFinishFlag(true);
}

bool Player::CheckCollisionWithClearBlock(CollisionMapInfo& info) {
	// 移動後の位置を計算
	Vector3 newPos = worldTransform_.translation_ + info.move;

	// クリアブロックの位置を取得
	int mapX = static_cast<int>(std::round(newPos.x / kBlockSize));
	int mapZ = static_cast<int>(std::round(newPos.z / kBlockSize));

	// マップ外や範囲外なら判定しない
	if (mapX < 0 || mapZ < 0 || mapX >= mapChipField_->GetMapWidth() || mapZ >= mapChipField_->GetMapHeight()) {
		return false;
	}

	// クリアブロックに当たった場合、ゲームクリア処理を呼び出す
	if (mapChipField_->GetMapChipTypeByIndex(mapX, mapZ) == MapChipType::kClear) {
		std::cout << "ゲームクリア！" << std::endl;
		OnGameClear(); // クリア時の処理を呼び出す
		return true;
	}

	return false;
}

bool Player::IsClear() const { return isClear_; }
