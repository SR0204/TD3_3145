#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include "Input.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player {

public://メンバ関数
	/// <summary>
	/// 解放処理
	/// </summary>
	~Player();
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

	WorldTransform worldTransform_;	//ワールド変換データ
	Model* model_ = nullptr;		//モデル
	Input* input_ = nullptr;		//入力
};