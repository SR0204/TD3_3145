#pragma once
#include "Audio.h"
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"

class MapChipField;
class GameScene;

class Player {
public:
	~Player();
	void Initialize(MapChipField* mapChipField, GameScene* gameScene);
	void Update();
	void Draw(ViewProjection& viewProjection);

private:
	void HandleInput();                     // 入力処理
	void ApplyMovement();                   // 移動処理
	void ProcessCollision();                // 衝突チェック
	void ProcessGameClear();                // クリア判定チェック
	bool DetectCollision(Vector3& outMove); // 衝突検出
	void OnGameClear();

private:
	Model* model_;
	WorldTransform worldTransform_;
	Vector3 moveVel_;
	Vector3 rotateVel_;
	Input* input_;
	MapChipField* mapChipField_;
	GameScene* gameScene_;
	bool isClear_ = false;
	float kWidth;
	float kDepth;
	uint32_t textureHandle_ = 0;
	float kBlockSize = 1.0f;
	float pi = 3.14159265358979323846f;
};
