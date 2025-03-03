#include "PlayerCamera.h"
#include "MathUtilityForText.h"
#include "ImGui.h"

void PlayerCamera::Initialize(Vector3 worldPos, Vector3 radian) { 
	worldTransform_.translation_ = worldPos;//引数のワールド座標を代入
	worldTransform_.rotation_ = radian;//引数の角度を代入
	viewProjection_.farZ = 300.0f;
	viewProjection_.Initialize();//ビュープロジェクションの初期化
	input_ = Input::GetInstance();// 入力のインスタンスを取得

	moveVel_ = 0.1f;
	rotateVel_ = 0.02f;
}

void PlayerCamera::Update() {
	//カメラの移動
	if (input_->PushKey(DIK_UP)) {
		worldTransform_.translation_.z += moveVel_;
	}
	if (input_->PushKey(DIK_DOWN)) {
		worldTransform_.translation_.z -= moveVel_;
	}
	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_.translation_.x -= moveVel_;
	}
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_.translation_.x += moveVel_;
	}
	
	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(false);
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

		// ImGuiで値を表示
	ImGui::Begin("Camera Status");

	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.01f);

	ImGui::End();
}

void PlayerCamera::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
