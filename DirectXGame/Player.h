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

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	/// <returns>ワールドトランスフォームを取得</returns>
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

private://メンバ変数
	WorldTransform worldTransform_;	//ワールド変換データ
	Model* model_ = nullptr;		//モデル
	uint32_t textureHandle_;		//テクスチャ
	Input* input_ = nullptr;		//入力

	float hitRad_;		//当たり判定の半径
	Vector3 rotateVel_;	//旋回速度
	float moveVel_;		//移動速度
};