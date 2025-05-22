#include "OverHeadCamera.h"
#include "MathUtilityForText.h"

void OverHeadCamera::Initialize() {
	worldTransform_.translation_ = {0.0f, 100.0f, 0.0f}; // 引数のワールド座標を代入
	worldTransform_.rotation_ = {1.57f, 0.0f, 0.0f};     // 引数の角度を代入
	viewProjection_.farZ = 600.0f;                       // farZの設定
	viewProjection_.Initialize();                        // ビュープロジェクションの初期化
}

void OverHeadCamera::Update() {
	// ワールド変換データの更新
	worldTransform_.UpdateMatrix(false);
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
}