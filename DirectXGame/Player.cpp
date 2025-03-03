#include "Player.h"

Player::~Player() {

}

void Player::Initialize() { 
	worldTransform_.Initialize();//ワールド変換データの初期化
	model_ = Model::CreateSphere();//モデルの生成
	input_ = Input::GetInstance();//入力のインスタンスを取得
}

void Player::Update() {

}

void Player::Draw(ViewProjection& viewProjection) { 
	//3Dモデルの描画
	model_->Draw(worldTransform_, viewProjection);
}
