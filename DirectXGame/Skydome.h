#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Skydome {

public://メンバ関数
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private://メンバ変数
	WorldTransform worldTransform_;	// ワールド変換データ
	Model* model_ = nullptr;// モデル
};
