#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
/// <summary>
/// 俯瞰カメラ
/// </summary>
class OverHeadCamera {

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
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクションを取得</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

private://メンバ関数
	WorldTransform worldTransform_; // ワールド変換データ
	ViewProjection viewProjection_; // ビュープロジェクション
};
