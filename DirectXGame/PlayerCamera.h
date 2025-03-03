#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"

/// <summary>
/// プレイヤーのカメラ
/// </summary>
class PlayerCamera {

public://メンバ関数
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(Vector3 worldPos,Vector3 radian);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	
	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクションを取得</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

private://メンバ変数
	Input* input_;//入力

	WorldTransform worldTransform_;//ワールド変換データ
	ViewProjection viewProjection_;//ビュープロジェクション

	float moveVel_;
	float rotateVel_;
};
