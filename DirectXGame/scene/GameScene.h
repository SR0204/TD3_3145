#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Map.h"
#include "Model.h"
#include "Player.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include<Sprite.h>
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <sstream>
#include"SkySphere.h"

#include "PlayerCamera.h"
#include "OverHeadCamera.h"

class GameScene {
private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	Map map;
	Player player;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	// 3Dモデルデータ
	Model* modelBlock_ = nullptr;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// `map.GetWidth()` と `map.GetHeight()` を使う
	uint32_t mapWidth = map.GetWidth();
	uint32_t mapHeight = map.GetHeight();

public:
	GameScene();
	~GameScene();
	void Initialize();
	void Update();
	void Draw();
	void GenerateBlocks();
};

#endif // GAMESCENE_H
