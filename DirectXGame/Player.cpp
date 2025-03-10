#define NOMINMAX
#include "Player.h"
#include "ImGui.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#define _USE_MATH_DEFINES
#include <MapChipField.h>
#include <math.h>

Player::~Player() { delete model_; }

void Player::Initialize() {
	worldTransform_.Initialize();                           // ワールド変換データの初期化
	model_ = Model::CreateFromOBJ("player");                // モデルの生成
	textureHandle_ = TextureManager::Load("uvChecker.png"); // テクスチャの読み込み
	input_ = Input::GetInstance();                          // 入力のインスタンスを取得

	vibrationVal_ = {};
	deadZone_ = 8000;     // デッドゾーン
	trigerDeadZone_ = 30; // トリガーのデッドゾーン

	leftStickLange_ = 0.0f;

	hitRad_ = 1.0f;                     // 当たり判定の半径
	rotateVel_ = {0.02f, 0.02f, 0.02f}; // 旋回速度
	moveVel_ = 0.1f;                    // 移動速度
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

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.5f, 1.0f));

	// コントローラーの状態をImGuiで出力
	ImGui::Begin("Controller");
	if (dwResult == ERROR_SUCCESS) {
		// コントローラーが接続出来ている時
		ImGui::Text("Connect");
		// ボタン入力を確認
		if (ImGui::TreeNode("Button")) {
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				ImGui::Text("A Buttun     [1]");
			} else {
				ImGui::Text("A Buttun     [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
				ImGui::Text("B Buttun     [1]");
			} else {
				ImGui::Text("B Buttun     [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
				ImGui::Text("X Buttun     [1]");
			} else {
				ImGui::Text("X Buttun     [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
				ImGui::Text("Y Buttun     [1]");
			} else {
				ImGui::Text("Y Buttun     [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
				ImGui::Text("LS Buttun    [1]");
			} else {
				ImGui::Text("LS Buttun    [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
				ImGui::Text("RS Buttun    [1]");
			} else {
				ImGui::Text("RS Buttun    [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
				ImGui::Text("START Buttun [1]");
			} else {
				ImGui::Text("START Buttun [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
				ImGui::Text("UP Buttun    [1]");
			} else {
				ImGui::Text("UP Buttun    [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
				ImGui::Text("DOWN Buttun  [1]");
			} else {
				ImGui::Text("DOWN Buttun  [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
				ImGui::Text("RIGHT Buttun [1]");
			} else {
				ImGui::Text("RIGHT Buttun [0]");
			}
			if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
				ImGui::Text("LEFT Buttun  [1]");
			} else {
				ImGui::Text("LEFT Buttun  [0]");
			}
			ImGui::TreePop();
		}
		// スティック
		if (ImGui::TreeNode("Stick")) {
			ImGui::Text("LeftStick  [%6d,%6d]", leftThumbX, leftThumbY);
			ImGui::Text("RightStick [%6d,%6d]", rightThumbX, rightThumbY);
			ImGui::Text("LeftStickLange [%f]", leftStickLange_);
			ImGui::Text("LeftUnitVector[%f,%f]", leftStickUnitVector_.x, leftStickUnitVector_.y);
			ImGui::Text("LeftStickLange [%f]", rightStickLange_);
			ImGui::Text("RightUnitVector[%f,%f]", rightStickUnitVector_.x, rightStickUnitVector_.y);
			ImGui::Text("DeadZone Max [32767]");
			ImGui::SliderInt("DeadZone", &deadZone_, 0, 32767);
			if (ImGui::TreeNode("Set")) {
				if (ImGui::Button("0", {50, 20})) {
					deadZone_ = 0;
				}
				if (ImGui::Button("2000", {50, 20})) {
					deadZone_ = 2000;
				}
				if (ImGui::Button("4000", {50, 20})) {
					deadZone_ = 4000;
				}
				if (ImGui::Button("6000", {50, 20})) {
					deadZone_ = 6000;
				}
				if (ImGui::Button("8000", {50, 20})) {
					deadZone_ = 8000;
				}
				if (ImGui::Button("10000", {50, 20})) {
					deadZone_ = 10000;
				}
				if (ImGui::Button("12000", {50, 20})) {
					deadZone_ = 12000;
				}
				if (ImGui::Button("14000", {50, 20})) {
					deadZone_ = 14000;
				}
				if (ImGui::Button("16000", {50, 20})) {
					deadZone_ = 16000;
				}
				if (ImGui::Button("18000", {50, 20})) {
					deadZone_ = 18000;
				}
				if (ImGui::Button("18000", {50, 20})) {
					deadZone_ = 18000;
				}
				if (ImGui::Button("20000", {50, 20})) {
					deadZone_ = 20000;
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		// トリガー
		if (ImGui::TreeNode("Trigger")) {
			ImGui::Text("LeftTriger [%3d]", leftTrigger);
			ImGui::Text("RightTriger[%3d]", rightTrigger);
			ImGui::Text("TriggerDeadZone Max[255]");
			ImGui::SliderInt("TriggerDeadZone", &trigerDeadZone_, 0, 255);
			if (ImGui::TreeNode("Set")) {
				if (ImGui::Button("0", {50, 20})) {
					trigerDeadZone_ = 0;
				}
				if (ImGui::Button("32", {50, 20})) {
					trigerDeadZone_ = 32;
				}
				if (ImGui::Button("64", {50, 20})) {
					trigerDeadZone_ = 64;
				}
				if (ImGui::Button("96", {50, 20})) {
					trigerDeadZone_ = 96;
				}
				if (ImGui::Button("128", {50, 20})) {
					trigerDeadZone_ = 128;
				}
				if (ImGui::Button("160", {50, 20})) {
					trigerDeadZone_ = 160;
				}
				if (ImGui::Button("192", {50, 20})) {
					trigerDeadZone_ = 192;
				}
				if (ImGui::Button("224", {50, 20})) {
					trigerDeadZone_ = 224;
				}
				if (ImGui::Button("255", {50, 20})) {
					trigerDeadZone_ = 255;
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		// 振動
		if (ImGui::TreeNode("Vibration")) {
			ImGui::Text("Vibration Max [65535]");
			ImGui::SliderFloat("VibrationX", &vibrationVal_.x, 0.0f, 65535.0f);
			ImGui::SliderFloat("VibrationY", &vibrationVal_.y, 0.0f, 65535.0f);
			ImGui::TreePop();
		}
		// リセット
		if (ImGui::TreeNode("Reset")) {
			ImGui::Text("DeadZone[8000]");
			ImGui::Text("TrigerDeadZone[30]");
			ImGui::Text("Vibration[0,0]");
			if (ImGui::Button("Reset")) {
				deadZone_ = 8000;
				trigerDeadZone_ = 30;
				vibrationVal_ = {};
			}
			ImGui::TreePop();
		}
	} else {
		// コントローラーが接続出来ていない時
		ImGui::Text("Unconnect");
	}

	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	/// ーーーここまでーーー

	const float pi = float(M_PI);

	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(true);

	// 旋回処理[I:上 K:下 L:右 J:左 ]
	if (input_->PushKey(DIK_K)) {
		worldTransform_.rotation_.x += rotateVel_.x;
	}
	if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.x -= rotateVel_.x;
	}
	if (input_->PushKey(DIK_L)) {
		worldTransform_.rotation_.y += rotateVel_.y;
	}
	if (input_->PushKey(DIK_J)) {
		worldTransform_.rotation_.y -= rotateVel_.y;
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

	// 移動処理(向いてる方向に進む)[W:前進 S:後退 D:右 A:左 ]
	if (input_->PushKey(DIK_W)) {
		// 座標に角度と移動速度を乗算した値を足す
		worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y) * moveVel_;
		worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y) * moveVel_;
	}
	if (input_->PushKey(DIK_S)) {
		worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y) * moveVel_;
		worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y) * moveVel_;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
		worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
	}
	if (input_->PushKey(DIK_A)) {
		worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
		worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
	}

	// 右スティックを使ったカメラ旋回
	if (rightStickUnitVector_.x > 0.0f || rightStickUnitVector_.y > 0.0f || rightStickUnitVector_.x < 0.0f || rightStickUnitVector_.y < 0.0f) {
		worldTransform_.rotation_.x += rightStickUnitVector_.y * -rotateVel_.x;
		worldTransform_.rotation_.y -= -rightStickUnitVector_.x * rotateVel_.y;
	}

	// 左スティックを使った移動
	if (leftThumbY > 0 && leftThumbY <= 32767) {
		worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y) * moveVel_;
		worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y) * moveVel_;
	}
	if (leftThumbY < 0 && leftThumbY >= -32768) {
		worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y) * moveVel_;
		worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y) * moveVel_;
	}
	if (leftThumbX > 0 && leftThumbX <= 32767) {
		worldTransform_.translation_.z += cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
		worldTransform_.translation_.x += sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
	}
	if (leftThumbX < 0 && leftThumbX >= -32768) {
		worldTransform_.translation_.z -= cosf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
		worldTransform_.translation_.x -= sinf(worldTransform_.rotation_.y + pi / 2.0f) * moveVel_;
	}

	// ImGuiで値を表示
	ImGui::Begin("Player");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);
	if (ImGui::Button("Reset")) {
		worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
		worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	}
	ImGui::End();

	// 衝突情報の初期化
	CollisionMapInfo collisionMapInfo;

	// 移動量に速度の値をコピー
	//collisionMapInfo.move = velocity_;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	//// 判定結果を反映して移動させる
	//CheckMapCollisionHit(collisionMapInfo);

	//CeilingContact(collisionMapInfo);

	//GroundedCondition(collisionMapInfo);

	//AnimateTurn();
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

// マップ衝突上判定
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真上の当たり判定
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	// MapChipField::IndexSet indexSet2;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +hitRad_ / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (hitRad_ / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.Ceiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	// 下降あり？
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真上の当たり判定
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	// MapChipField::IndexSet indexSet2;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +hitRad_ / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (hitRad_ / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.landing = true;
	}
}

// 右
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	// 右あり？
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真上の当たり判定
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	// MapChipField::IndexSet indexSet2;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+hitRad_ / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (hitRad_ / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.Ceiling = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	// 右あり？
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真上の当たり判定
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	// MapChipField::IndexSet indexSet2;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-hitRad_ / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (hitRad_ / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.Ceiling = true;
	}
}

// 接地状態
// void Player::GroundedCondition(const CollisionMapInfo& info) {
//
//	// 自キャラが接地状態?
//	if (onGround_) {
//		// 接地状態の処理
//		// ジャンプ開始
//		if (velocity_.y > 0.0f) {
//			onGround_ = false;
//		} else {
//
//			// 移動後の四つの角の座標
//			std::array<Vector3, kNumCorner> positionNew;
//
//			for (uint32_t i = 0; i < positionNew.size(); ++i) {
//				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
//			}
//
//			MapChipType mapChipType;
//
//			// 真下の当たり判定
//			bool hit = false;
//
//			// 左下点の判定
//			MapChipField::IndexSet indexSet;
//			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + Vector3(0, -kAttenuationShift, 0));
//			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//			if (mapChipType == MapChipType::kBlock) {
//				hit = true;
//			}
//
//			// 右下点の判定
//			//  MapChipField::IndexSet indexSet2;
//			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom] + Vector3(0, -kAttenuationShift, 0));
//			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//			if (mapChipType == MapChipType::kBlock) {
//				hit = true;
//			}
//
//			// 落下開始
//			if (!hit) {
//				// 空中状態に切り替える
//				onGround_ = false;
//			}
//		}
//	} else {
//		// 空中状態
//		// 着地フラグ
//		if (info.landing) {
//			// 着地状態に切り替える
//			onGround_ = true;
//
//			// 着地時にX速度を減衰
//			velocity_.x *= (1.0f - kAttenuationLanding);
//
//			// Y速度をゼロにする
//			velocity_.y = 0.0f;
//		}
//	}
//}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+hitRad_ / 2.0f, -hitRad_ / 2.0f, 0}, //  kRightBottom
	    {-hitRad_ / 2.0f, -hitRad_ / 2.0f, 0}, //  kLeftBottom
	    {+hitRad_ / 2.0f, +hitRad_ / 2.0f, 0}, //  kRightTop
	    {-hitRad_ / 2.0f, +hitRad_ / 2.0f, 0}, //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

// void Player::CeilingContact(const CollisionMapInfo& info) {
//
//	// 天井に当たった？
//	if (info.Ceiling) {
//		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
//		velocity_.y = 0;
//	}
// }
//
// void Player::CheckMapCollisionHit(const CollisionMapInfo& info) {
//
//	// 移動
//	worldTransform_.translation_ += info.move;
// }
