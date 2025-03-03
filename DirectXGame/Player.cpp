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

	hitRad_ = 1.0f;//当たり判定の半径
	rotateVel_ = {0.02f, 0.02f, 0.02f};//旋回速度
	moveVel_ = 0.1f;//移動速度
}

void Player::Update() {

	const float pi = float(M_PI);

	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(true);

	//旋回処理[I:上 K:下 J:左 L:右]
	if (input_->PushKey(DIK_K)) {
		worldTransform_.rotation_.x += rotateVel_.x;
	}
	if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.x -= rotateVel_.x;
	}
	if (input_->PushKey(DIK_J)){
		worldTransform_.rotation_.y -= rotateVel_.y;
	}
	if (input_->PushKey(DIK_L)) {
		worldTransform_.rotation_.y += rotateVel_.y;
	}

	//旋回値が|π|を越えたら値を変える
	if (worldTransform_.rotation_.y >= pi || worldTransform_.rotation_.y <= -pi) {
		worldTransform_.rotation_.y *= -1;
	}

	//移動処理(向いてる方向に進む)[W:前進 S:後退 A:左 D:右]
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
	ImGui::End();
}

void Player::Draw(ViewProjection& viewProjection) { 
	//3Dモデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
