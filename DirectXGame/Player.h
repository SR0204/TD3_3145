#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"

class MapChipField;

/// <summary>
/// プレイヤー
/// </summary>
class Player {

public: // メンバ関数
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

	void SetTranslation(Vector3 translation) { worldTransform_.translation_ = translation; }

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

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool Ceiling = false; // 天井衝突フラグ
		bool landing = false; // 着地フラグ
		bool HitWall = false; // 壁接触フラグ
		Vector3 move;         // 移動量
	};

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	static inline const float kBlank = 1.0f;

	// 判定結果を反映して移動させる
	//void CheckMapCollisionHit(const CollisionMapInfo& info);

	// 天井に接触している場合の処理
	//void CeilingContact(const CollisionMapInfo& info);

private:                            // メンバ変数
	WorldTransform worldTransform_; // ワールド変換データ
	Model* model_ = nullptr;        // モデル
	uint32_t textureHandle_;        // テクスチャ
	Input* input_ = nullptr;        // 入力

	XINPUT_STATE state_;         // コントローラーの状態
	XINPUT_VIBRATION vibration_; // バイブレーション値
	Vector2 vibrationVal_;       // 調整用バイブレーション値
	int deadZone_;               // デッドゾーン
	int trigerDeadZone_;         // トリガーのデッドゾーン

	float leftStickLange_;         // 左スティックのベクトルの長さ
	Vector2 leftStickUnitVector_;  // 左単位ベクトル
	float rightStickLange_;        // 右スティックのベクトルの長さ
	Vector2 rightStickUnitVector_; // 右単位ベクトル

	float hitRad_;      // 当たり判定の半径
	Vector3 rotateVel_; // 旋回速度
	float moveVel_;     // 移動速度

	// マップチップのフィールド
	MapChipField* mapChipField_;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};