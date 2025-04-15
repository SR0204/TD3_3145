#define NOMINMAX
#include "Player.h"
// #include "ImGui.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#define _USE_MATH_DEFINES
#include "GameScene.h"
#include "MapChipField.h"
#include <cassert>
#include <iostream>
#include <math.h>

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
}

void Player::Update() {

	// コントローラー関係
	/// ーーーここからーーー

	//DWORD dwResult = XInputGetState(0, &state_);                      // コントローラーの状態を取得
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

	// ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
	// ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.5f, 1.0f));

	//// コントローラーの状態をImGuiで出力
	// ImGui::Begin("Controller");
	// if (dwResult == ERROR_SUCCESS) {
	//	// コントローラーが接続出来ている時
	//	ImGui::Text("Connect");
	//	// ボタン入力を確認
	//	if (ImGui::TreeNode("Button")) {
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
	//			ImGui::Text("A Buttun     [1]");
	//		} else {
	//			ImGui::Text("A Buttun     [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
	//			ImGui::Text("B Buttun     [1]");
	//		} else {
	//			ImGui::Text("B Buttun     [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
	//			ImGui::Text("X Buttun     [1]");
	//		} else {
	//			ImGui::Text("X Buttun     [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
	//			ImGui::Text("Y Buttun     [1]");
	//		} else {
	//			ImGui::Text("Y Buttun     [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
	//			ImGui::Text("LS Buttun    [1]");
	//		} else {
	//			ImGui::Text("LS Buttun    [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
	//			ImGui::Text("RS Buttun    [1]");
	//		} else {
	//			ImGui::Text("RS Buttun    [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
	//			ImGui::Text("START Buttun [1]");
	//		} else {
	//			ImGui::Text("START Buttun [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
	//			ImGui::Text("UP Buttun    [1]");
	//		} else {
	//			ImGui::Text("UP Buttun    [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
	//			ImGui::Text("DOWN Buttun  [1]");
	//		} else {
	//			ImGui::Text("DOWN Buttun  [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
	//			ImGui::Text("RIGHT Buttun [1]");
	//		} else {
	//			ImGui::Text("RIGHT Buttun [0]");
	//		}
	//		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
	//			ImGui::Text("LEFT Buttun  [1]");
	//		} else {
	//			ImGui::Text("LEFT Buttun  [0]");
	//		}
	//		ImGui::TreePop();
	//	}
	//	// スティック
	//	if (ImGui::TreeNode("Stick")) {
	//		ImGui::Text("LeftStick  [%6d,%6d]", leftThumbX, leftThumbY);
	//		ImGui::Text("RightStick [%6d,%6d]", rightThumbX, rightThumbY);
	//		ImGui::Text("LeftStickLange [%f]", leftStickLange_);
	//		ImGui::Text("LeftUnitVector[%f,%f]", leftStickUnitVector_.x, leftStickUnitVector_.y);
	//		ImGui::Text("LeftStickLange [%f]", rightStickLange_);
	//		ImGui::Text("RightUnitVector[%f,%f]", rightStickUnitVector_.x, rightStickUnitVector_.y);
	//		ImGui::Text("DeadZone Max [32767]");
	//		ImGui::SliderInt("DeadZone", &deadZone_, 0, 32767);
	//		if (ImGui::TreeNode("Set")) {
	//			if (ImGui::Button("0", {50, 20})) {
	//				deadZone_ = 0;
	//			}
	//			if (ImGui::Button("2000", {50, 20})) {
	//				deadZone_ = 2000;
	//			}
	//			if (ImGui::Button("4000", {50, 20})) {
	//				deadZone_ = 4000;
	//			}
	//			if (ImGui::Button("6000", {50, 20})) {
	//				deadZone_ = 6000;
	//			}
	//			if (ImGui::Button("8000", {50, 20})) {
	//				deadZone_ = 8000;
	//			}
	//			if (ImGui::Button("10000", {50, 20})) {
	//				deadZone_ = 10000;
	//			}
	//			if (ImGui::Button("12000", {50, 20})) {
	//				deadZone_ = 12000;
	//			}
	//			if (ImGui::Button("14000", {50, 20})) {
	//				deadZone_ = 14000;
	//			}
	//			if (ImGui::Button("16000", {50, 20})) {
	//				deadZone_ = 16000;
	//			}
	//			if (ImGui::Button("18000", {50, 20})) {
	//				deadZone_ = 18000;
	//			}
	//			if (ImGui::Button("18000", {50, 20})) {
	//				deadZone_ = 18000;
	//			}
	//			if (ImGui::Button("20000", {50, 20})) {
	//				deadZone_ = 20000;
	//			}
	//			ImGui::TreePop();
	//		}
	//		ImGui::TreePop();
	//	}
	//	// トリガー
	//	if (ImGui::TreeNode("Trigger")) {
	//		ImGui::Text("LeftTriger [%3d]", leftTrigger);
	//		ImGui::Text("RightTriger[%3d]", rightTrigger);
	//		ImGui::Text("TriggerDeadZone Max[255]");
	//		ImGui::SliderInt("TriggerDeadZone", &trigerDeadZone_, 0, 255);
	//		if (ImGui::TreeNode("Set")) {
	//			if (ImGui::Button("0", {50, 20})) {
	//				trigerDeadZone_ = 0;
	//			}
	//			if (ImGui::Button("32", {50, 20})) {
	//				trigerDeadZone_ = 32;
	//			}
	//			if (ImGui::Button("64", {50, 20})) {
	//				trigerDeadZone_ = 64;
	//			}
	//			if (ImGui::Button("96", {50, 20})) {
	//				trigerDeadZone_ = 96;
	//			}
	//			if (ImGui::Button("128", {50, 20})) {
	//				trigerDeadZone_ = 128;
	//			}
	//			if (ImGui::Button("160", {50, 20})) {
	//				trigerDeadZone_ = 160;
	//			}
	//			if (ImGui::Button("192", {50, 20})) {
	//				trigerDeadZone_ = 192;
	//			}
	//			if (ImGui::Button("224", {50, 20})) {
	//				trigerDeadZone_ = 224;
	//			}
	//			if (ImGui::Button("255", {50, 20})) {
	//				trigerDeadZone_ = 255;
	//			}
	//			ImGui::TreePop();
	//		}
	//		ImGui::TreePop();
	//	}
	//	// 振動
	//	if (ImGui::TreeNode("Vibration")) {
	//		ImGui::Text("Vibration Max [65535]");
	//		ImGui::SliderFloat("VibrationX", &vibrationVal_.x, 0.0f, 65535.0f);
	//		ImGui::SliderFloat("VibrationY", &vibrationVal_.y, 0.0f, 65535.0f);
	//		ImGui::TreePop();
	//	}
	//	// リセット
	//	if (ImGui::TreeNode("Reset")) {
	//		ImGui::Text("DeadZone[8000]");
	//		ImGui::Text("TrigerDeadZone[30]");
	//		ImGui::Text("Vibration[0,0]");
	//		if (ImGui::Button("Reset")) {
	//			deadZone_ = 8000;
	//			trigerDeadZone_ = 30;
	//			vibrationVal_ = {};
	//		}
	//		ImGui::TreePop();
	//	}
	// } else {
	//	// コントローラーが接続出来ていない時
	//	ImGui::Text("Unconnect");
	// }

	// ImGui::End();

	// ImGui::PopStyleColor();
	// ImGui::PopStyleColor();

	/// ーーーここまでーーー

	const float pi = float(M_PI);

	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(true);

	// クリア判定
	if (mapChipField_->IsClearBlock(worldTransform_.translation_) == true) {
		std::cout << "ゲームクリア！" << std::endl;
		// クリア時の処理をここに追加（例: ステージ移行、リスタートなど）
		gameScene_->IsClear();
	}

	// 移動情報をセット
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = moveVel_;
	collisionMapInfo.isBlocked = false; // 初期状態ではブロックされていない

	// 移動処理を行う前に衝突判定を行う
	if (CheckMapCollision(collisionMapInfo) || CheckCollisionWithCSVMap(collisionMapInfo)) {
		// 衝突している場合、移動を停止
		collisionMapInfo.isBlocked = true;
		moveVel_ = {0.0f, 0.0f, 0.0f}; // 移動を停止
	}

	// 旋回処理 [I:上 K:下 L:右 J:左]
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

	// 旋回時に再び移動を許可
	if (rotated) {
		collisionMapInfo.isBlocked = false;
		moveVel_ = {0.1f, 0.0f, 0.1f}; // 移動速度を復元
	}

	// 回転処理が終わった後でrotatedをfalseにリセット
	if (!input_->PushKey(DIK_K) && !input_->PushKey(DIK_I) && !input_->PushKey(DIK_L) && !input_->PushKey(DIK_J)) {
		rotated = false;
	}

	// 旋回値が|π|を越えたら値を変える
	if (worldTransform_.rotation_.y >= pi || worldTransform_.rotation_.y <= -pi) {
		worldTransform_.rotation_.y *= -1;
	}

	// カメラの角度制限
	if (worldTransform_.rotation_.x > pi / 4.0f) {
		worldTransform_.rotation_.x = pi / 4.0f;
	}
	if (worldTransform_.rotation_.x < -pi / 4.0f) {
		worldTransform_.rotation_.x = -pi / 4.0f;
	}

	// 視点の荒ぶりを防ぐために、移動方向のベクトルに影響するように移動処理を調整
	if (!collisionMapInfo.isBlocked) {
		// 向いている方向に進む
		if (input_->PushKey(DIK_W)) {
			worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y) * moveVel_.z;
			worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y) * moveVel_.x;
		}
		if (input_->PushKey(DIK_S)) {
			worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y) * moveVel_.z;
			worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y) * moveVel_.x;
		}
		if (input_->PushKey(DIK_D)) {
			worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.z;
			worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.x;
		}
		if (input_->PushKey(DIK_A)) {
			worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.z;
			worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.x;
		}
	}

	// 右スティックを使ったカメラ旋回
	if (rightStickUnitVector_.x > 0.0f || rightStickUnitVector_.y > 0.0f || rightStickUnitVector_.x < 0.0f || rightStickUnitVector_.y < 0.0f) {
		worldTransform_.rotation_.x += rightStickUnitVector_.y * -rotateVel_.x;
		worldTransform_.rotation_.y -= -rightStickUnitVector_.x * rotateVel_.y;
	}

	// 左スティックを使った移動
	if (leftThumbY > 0 && leftThumbY <= 32767) {
		worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y) * moveVel_.x;
		worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y) * moveVel_.z;
	}
	if (leftThumbY < 0 && leftThumbY >= -32768) {
		worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y) * moveVel_.z;
		worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y) * moveVel_.x;
	}
	if (leftThumbX > 0 && leftThumbX <= 32767) {
		worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.z;
		worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.x;
	}
	if (leftThumbX < 0 && leftThumbX >= -32768) {
		worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.z;
		worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_.x;
	}

	// ImGuiで値を表示
	/*ImGui::Begin("Player");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);
	if (ImGui::Button("Reset")) {
	    worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	    worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	}
	ImGui::End();*/

	// キーが放された場合に減速処理を行う
	// const float deceleration = 0.05f;
	// if (!input_->PushKey(DIK_W) && !input_->PushKey(DIK_S)) {
	//	if (moveVel_.z > 0.0f) {
	//		moveVel_.z = std::max(0.0f, moveVel_.z - deceleration);
	//	} else if (moveVel_.z < 0.0f) {
	//		moveVel_.z = std::min(0.0f, moveVel_.z + deceleration);
	//	}
	//}

	// if (!input_->PushKey(DIK_A) && !input_->PushKey(DIK_D)) {
	//	if (moveVel_.x > 0.0f) {
	//		moveVel_.x = std::max(0.0f, moveVel_.x - deceleration);
	//	} else if (moveVel_.x < 0.0f) {
	//		moveVel_.x = std::min(0.0f, moveVel_.x + deceleration);
	//	}
	// }
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

bool Player::CheckMapCollision(CollisionMapInfo& info) {
	// 通常の衝突判定
	if (CheckCollisionWithCSVMap(info)) {
		return true;
	}

	// 各方向の衝突判定をまとめて行う
	if (CheckMapCollisionDirection(info, Vector3(kWidth, 0, 0)) ||  // 右
	    CheckMapCollisionDirection(info, Vector3(-kWidth, 0, 0)) || // 左
	    CheckMapCollisionDirection(info, Vector3(0, 0, kDepth)) ||  // 前進
	    CheckMapCollisionDirection(info, Vector3(0, 0, -kDepth))) { // 後退
		return true;
	}

	// クリアブロックとの衝突判定
	if (CheckCollisionWithClearBlock(info)) {
		return true;
	}

	return false;
}


// 右方向の衝突判定
//bool Player::CheckMapCollisionRight(CollisionMapInfo& info) {
//	if (info.move.x <= 0) {
//		return false;
//	}
//
//	// 四隅の座標を修正
//	std::array<Vector3, 4> positionNew = {
//	    worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, +kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, +kDepth / 2.0f),
//	};
//
//	bool hit = false;
//	MapChipField::IndexSet indexSet;
//	for (const auto& pos : positionNew) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
//		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.zIndex) == MapChipType::kBlock) {
//			hit = true;
//			break;
//		}
//	}
//
//	if (hit) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.zIndex);
//		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
//		info.hitWall = true;
//
//		// デバッグログ
//		std::cout << "Collision Right! Adjusted X: " << info.move.x << std::endl;
//		return true;
//	}
//
//	return false;
//}

// 左方向の衝突判定
//bool Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
//	if (info.move.x >= 0) {
//		return false;
//	}
//
//	// 四隅の座標を修正
//	std::array<Vector3, 4> positionNew = {
//	    worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, +kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, +kDepth / 2.0f),
//	};
//
//	bool hit = false;
//	MapChipField::IndexSet indexSet;
//	for (const auto& pos : positionNew) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
//		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.zIndex) == MapChipType::kBlock) {
//			hit = true;
//			break;
//		}
//	}
//
//	if (hit) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));
//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.zIndex);
//		info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank * 0.5f));
//		info.hitWall = true;
//		return true;
//	}
//
//	return false;
//}

bool Player::CheckMapCollisionDirection(CollisionMapInfo& info, const Vector3& direction) {
	if (direction.x != 0 && (info.move.x <= 0 && direction.x > 0 || info.move.x >= 0 && direction.x < 0)) {
		return false;
	}
	if (direction.z != 0 && (info.move.z <= 0 && direction.z > 0 || info.move.z >= 0 && direction.z < 0)) {
		return false;
	}

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
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + direction);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.zIndex);
		if (direction.x != 0) {
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		}
		if (direction.z != 0) {
			info.move.z = std::max(0.0f, rect.front - worldTransform_.translation_.z - (kDepth / 2.0f + kBlank * 0.5f));
		}
		info.hitWall = true;

		// デバッグログ
		std::cout << "Collision " << direction.x << ", " << direction.z << "!" << std::endl;
		return true;
	}

	return false;
}

// 前進方向（Z+）の衝突判定
//bool Player::CheckMapCollisionForward(CollisionMapInfo& info) {
//	if (info.move.z <= 0) {
//		return false;
//	}
//
//	// 四隅の座標を修正
//	std::array<Vector3, 4> positionNew = {
//	    worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, +kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, +kDepth / 2.0f),
//	};
//
//	bool hit = false;
//	MapChipField::IndexSet indexSet;
//	for (const auto& pos : positionNew) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
//		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.zIndex) == MapChipType::kBlock) {
//			hit = true;
//			break;
//		}
//	}
//
//	if (hit) {
//		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, 0, +kDepth / 2.0f));
//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.zIndex);
//		info.move.z = std::max(0.0f, rect.front - worldTransform_.translation_.z - (kDepth / 2.0f + kBlank * 0.5f));
//		info.hitWall = true;
//		return true;
//	}
//
//	return false;
//}

// 後退方向（Z-）の衝突判定
//bool Player::CheckMapCollisionBackward(CollisionMapInfo& info) {
//	// 前方向の移動なら処理不要
//	if (info.move.z >= 0) {
//		return false;
//	}
//
//	// 後退方向の4つのコーナーの座標
//	// 四隅の座標を修正
//	std::array<Vector3, 4> positionNew = {
//	    worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
//	    worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
//	};
//
//	// 衝突判定
//	bool hit = false;
//	for (const auto& pos : positionNew) {
//		auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
//		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.zIndex) == MapChipType::kBlock) {
//			hit = true;
//			break;
//		}
//	}
//
//	// 衝突時の処理
//	if (hit) {
//		auto indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, 0, -kDepth / 2.0f));
//		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.zIndex);
//
//		// プレイヤーの後ろ方向への移動を制限
//		info.move.z = std::min(0.0f, rect.back - worldTransform_.translation_.z + (kDepth / 2.0f + kBlank * 0.5f));
//
//		info.hitWall = true;
//		return true;
//	}
//
//	return false;
//}

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
	// プレイヤーの四隅をチェック
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

		// マップ外や壁なら衝突判定
		if (mapX < 0 || mapZ < 0 || mapX >= mapChipField_->GetMapWidth() || mapZ >= mapChipField_->GetMapHeight()) {
			hitX = true;
			hitZ = true;
			continue;
		}

		// マップチップの種類を取得
		MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(mapX, mapZ);

		// 壁ブロックなら衝突処理
		if (chipType == MapChipType::kBlock) {
			if (corner.x != worldTransform_.translation_.x) {
				hitX = true;
			}
			if (corner.z != worldTransform_.translation_.z) {
				hitZ = true;
			}
		}

		// **クリアブロックならゲームクリア処理**
		if (chipType == MapChipType::kClear) {
			std::cout << "ゲームクリア！" << std::endl;
			OnGameClear(); // クリア時の処理を呼び出す
			return true;
		}
	}

	// X方向の衝突があったらX成分をゼロにする
	if (hitX) {
		info.move.x = 0;
	}

	// Z方向の衝突があったらZ成分をゼロにする
	if (hitZ) {
		info.move.z = 0;
	}

	info.hitWall = hitX || hitZ;
	return info.hitWall;
}

void Player::OnGameClear() {}

bool Player::CheckCollisionWithClearBlock(CollisionMapInfo& info) {
	Vector3 centerPos = worldTransform_.translation_ + info.move;
	int mapX = static_cast<int>(std::round(centerPos.x / kBlockSize));
	int mapZ = static_cast<int>(std::round(centerPos.z / kBlockSize));

	if (mapX < 0 || mapZ < 0 || mapX >= mapChipField_->GetMapWidth() || mapZ >= mapChipField_->GetMapHeight()) {
		return false;
	}

	// クリアブロックに当たったらリセット
	if (mapChipField_->GetMapChipTypeByIndex(mapX, mapZ) == MapChipType::kClear) {
		std::cout << "ゴール！プレイヤーリセット！" << std::endl;
		isClear_ = true;
		return true;
	}

	return false;
}
