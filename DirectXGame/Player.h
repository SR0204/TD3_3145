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

	/// <summary>
	/// デッドゾーンの処理
	/// </summary>
	/// <param name="axis"></param>
	void ApplyDeadZone(int& axis) {
		if (axis > -deadZone_ && axis < deadZone_) {
			axis = 0; // デッドゾーン内の値を無視
		}
	}

	/// <summary>
	/// トリガーのデッドゾーンを適用
	/// </summary>
	/// <param name="trigger"></param>
	void ApplyTriggerDeadZone(BYTE& trigger) {
		if (trigger < trigerDeadZone_) {
			trigger = 0;
		}
	}

private://メンバ変数
	WorldTransform worldTransform_;	//ワールド変換データ
	Model* model_ = nullptr;		//モデル
	uint32_t textureHandle_;		//テクスチャ
	Input* input_ = nullptr;		//入力

	XINPUT_STATE state_;         // コントローラーの状態
	XINPUT_VIBRATION vibration_; // バイブレーション値
	Vector2 vibrationVal_;//調整用バイブレーション値
	int deadZone_;//デッドゾーン
	int trigerDeadZone_;//トリガーのデッドゾーン
	
	float leftStickLange_;//左スティックのベクトルの長さ
	Vector2 leftStickUnitVector_;//左単位ベクトル
	float rightStickLange_;        // 右スティックのベクトルの長さ
	Vector2 rightStickUnitVector_; // 右単位ベクトル

	float hitRad_;		//当たり判定の半径
	Vector3 rotateVel_;	//旋回速度
	float moveVel_;		//移動速度
};