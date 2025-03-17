#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Map.h"
#include "Model.h"
#include "Player.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class GameScene {
private:
	Map map;
	Player player;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	// 3Dモデルデータ
	Model* modelBlock_ = nullptr;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	

public:
	GameScene();
	~GameScene();
	void Initialize();
	void Update();
	void Draw(); // ここにDirectXの描画処理を後で追加
	void GenerateBlocks();
};

#endif // GAMESCENE_H
