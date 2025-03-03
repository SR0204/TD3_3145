#include "Skydome.h"
#include "cassert"

Skydome::~Skydome() { delete model_; }

void Skydome::Initialize() {
	// 引数で受け取ったデータをメンバ変数に記録
	model_ = Model::CreateFromOBJ("skydome");
	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Skydome::Update() {
	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix(true);
}

void Skydome::Draw(ViewProjection& viewProjection) { model_->Draw(worldTransform_, viewProjection); }
