#include "WorldTransform.h"
#include "MathUtilityForText.h"

void WorldTransform::UpdateMatrix(bool isTransfer) {
	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}

	if (isTransfer == true) {
		// 定数バッファに転送
		TransferMatrix();
	}
}