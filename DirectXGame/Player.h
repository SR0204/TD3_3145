#pragma once
#include "Audio.h"
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"

class MapChipField;
class GameScene;

/// <summary>
/// プレイヤー
/// </summary>
class Player {

public: // メンバ関数
	Player();

	/// <summary>
	/// 解放処理
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Vector3& position);

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
		bool hitWall = false; // 壁接触フラグ
		Vector3 move;         // 移動量
		bool isBlocked = false;
	};

	bool CheckMapCollision(CollisionMapInfo& info);

	bool CheckMapCollisionDirection(CollisionMapInfo& info, const Vector3& direction);

	// 角
	enum Corner {
		kRightBottomFront, // 右下前
		kLeftBottomFront,  // 左下前
		kRightTopFront,    // 右上前
		kLeftTopFront,     // 左上前
		kRightBottomBack,  // 右下後
		kLeftBottomBack,   // 左下後
		kRightTopBack,     // 右上後
		kLeftTopBack,      // 左上後
		kNumCorner         // 要素数
	};

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	static inline const float kBlank = 1.0f;

	bool CheckCollisionWithCSVMap(CollisionMapInfo& info);

	bool CheckCollisionWithClearBlock(CollisionMapInfo& info);

	void OnGameClear();

	int GetHp() { return hp_; }

private: // メンバ変数
	Audio* audio_ = nullptr;
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

	// キャラクターの当たり判定サイズ
	float kWidth;
	float kHeight;
	float kDepth;       // プレイヤーの奥行き（新しく追加）
	Vector3 rotateVel_; // 旋回速度
	Vector3 moveVel_;   // 移動速度

	// マップチップのフィールド
	MapChipField* mapChipField_ = nullptr;
	float kBlockSize = 1.0f;

	bool canMoveAfterCollision;

	GameScene* gameScene_;

	// プレイヤーの初期位置を記憶
	Vector3 startPosition = {14.0f, 2.0f, 3.0f};
	int hp_ = 100;

	// 終了フラグ
	bool isFinished = false;

	bool isClear_ = false;
	// 音
	uint32_t music;

	// 音声再生
	uint32_t playMusic;
};