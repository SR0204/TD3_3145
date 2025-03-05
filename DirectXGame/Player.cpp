#include "Player.h"
#include "TextureManager.h"
#include "MathUtilityForText.h"
#include "ImGui.h"
#define _USE_MATH_DEFINES
#include <math.h>

Player::~Player() { 
	delete model_;
}

void Player::Initialize() { 
	worldTransform_.Initialize();//ワールド変換データの初期化
	model_ = Model::CreateFromOBJ("player");// モデルの生成
	textureHandle_ = TextureManager::Load("uvChecker.png"); // テクスチャの読み込み
	input_ = Input::GetInstance();//入力のインスタンスを取得

	vibrationVal_ = {};
	deadZone_ = 8000;//デッドゾーン
	trigerDeadZone_ = 30;//トリガーのデッドゾーン

	hitRad_ = 1.0f;//当たり判定の半径
	rotateVel_ = {0.02f, 0.02f, 0.02f};//旋回速度
	moveVel_ = 0.1f;//移動速度
}

void Player::Update() {

	//コントローラー関係
	///ーーーここからーーー

	DWORD dwResult = XInputGetState(0, &state_);//コントローラーの状態を取得
	vibration_.wLeftMotorSpeed = static_cast<WORD>(vibrationVal_.x);  // 左モーターの振動強さ（最大値）
	vibration_.wRightMotorSpeed = static_cast<WORD>(vibrationVal_.y); // 右モーターの振動強さ（最大値）
	XInputSetState(0, &vibration_); // コントローラー0のバイブレーションを設定
	int leftThumbX = state_.Gamepad.sThumbLX;//左スティックのX値
	int leftThumbY = state_.Gamepad.sThumbLY;//左スティックのY値
	int rightThumbX = state_.Gamepad.sThumbRX;//右スティックのX値
	int rightThumbY = state_.Gamepad.sThumbRY;//右スティックのX値
	BYTE leftTrigger = state_.Gamepad.bLeftTrigger;//左トリガーの値
	BYTE rightTrigger = state_.Gamepad.bRightTrigger;//右トリガーの値

	// デッドゾーンの適用
	ApplyDeadZone(leftThumbX);
	ApplyDeadZone(leftThumbY);
	ApplyDeadZone(rightThumbX);
	ApplyDeadZone(rightThumbY);
	ApplyTriggerDeadZone(leftTrigger);
	ApplyTriggerDeadZone(rightTrigger);


	//コントローラーの状態をImGuiで出力
	ImGui::Begin("Controller");
	if (dwResult == ERROR_SUCCESS){
		// コントローラーが接続出来ている時
		ImGui::Text("Connect");
		//ボタン入力を確認
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			ImGui::Text("A Buttun is Push");
		} else {
			ImGui::Text("A Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			ImGui::Text("B Buttun is Push");
		} else {
			ImGui::Text("B Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			ImGui::Text("X Buttun is Push");
		} else {
			ImGui::Text("X Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
			ImGui::Text("Y Buttun is Push");
		} else {
			ImGui::Text("Y Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
			ImGui::Text("LS Buttun is Push");
		} else {
			ImGui::Text("LS Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			ImGui::Text("RS Buttun is Push");
		} else {
			ImGui::Text("RS Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
			ImGui::Text("START Buttun is Push");
		} else {
			ImGui::Text("START Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
			ImGui::Text("UP Buttun is Push");
		} else {
			ImGui::Text("UP Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
			ImGui::Text("DOWN Buttun is Push");
		} else {
			ImGui::Text("DOWN Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
			ImGui::Text("RIGHT Buttun is Push");
		} else {
			ImGui::Text("RIGHT Buttun is Release");
		}
		if (state_.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
			ImGui::Text("LEFT Buttun is Push");
		} else {
			ImGui::Text("LEFT Buttun is Release");
		}
		ImGui::Text("leftThumb  [%6d,%6d]", leftThumbX, leftThumbY);
		ImGui::Text("rightThumb [%6d,%6d]", rightThumbX, rightThumbY);
		ImGui::Text("leftTriger [%3d]", leftTrigger);
		ImGui::Text("rightTriger[%3d]", rightTrigger);
		ImGui::Text("DeadZone Max [32767]");
		ImGui::DragInt("DeadZone", &deadZone_, 1, 0, 32767);
		ImGui::Text("TriggerDeadZone Max[255]");
		ImGui::DragInt("TriggerDeadZone", &trigerDeadZone_, 1, 0, 255);
		ImGui::Text("Vibration Max [65535]");
		ImGui::DragFloat2("Vibration", &vibrationVal_.x, 100.0f, 0.0f, 65535.0f);
	} else {
		// コントローラーが接続出来ていない時
		ImGui::Text("Unconnect");
	}

	ImGui::End();

	///ーーーここまでーーー

	const float pi = float(M_PI);

	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(true);

	//旋回処理[I:上 K:下 L:右 J:左 ]
	if (input_->PushKey(DIK_K)) {
		worldTransform_.rotation_.x += rotateVel_.x;
	}
	if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.x -= rotateVel_.x;
	}
	if (input_->PushKey(DIK_L)) {
		worldTransform_.rotation_.y += rotateVel_.y;
	}
	if (input_->PushKey(DIK_J)){
		worldTransform_.rotation_.y -= rotateVel_.y;
	}
	

	//旋回値が|π|を越えたら値を変える
	if (worldTransform_.rotation_.y >= pi || worldTransform_.rotation_.y <= -pi) {
		worldTransform_.rotation_.y *= -1;
	}

	//移動処理(向いてる方向に進む)[W:前進 S:後退 D:右 A:左 ]
	if (input_->PushKey(DIK_W)){
		//座標に角度と移動速度を乗算した値を足す
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

	//ImGuiで値を表示
	ImGui::Begin("Player Status");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);
	if (ImGui::Button("Reset")) {
		worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
		worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	}
	ImGui::End();
}

void Player::Draw(ViewProjection& viewProjection) { 
	//3Dモデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
