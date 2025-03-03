#include "SkySphere.h"
#include <cassert>

SkySphere::SkySphere() {}

SkySphere::~SkySphere() {}

void SkySphere::Initialize(Model* model, ViewProjection* viewProjection) {

	// NULLポインタチェック
	assert(model);

	// 引数
	modelSykDome_ = model;

	viewProjection_ = viewProjection;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	viewProjection_ = viewProjection;
}

void SkySphere::Update() {}

void SkySphere::Draw() {

	// 3Dモデルの描画
	modelSykDome_->Draw(worldTransform_, *viewProjection_);
}
