#include "GameScene.h"
#include <iostream>
#include <windows.h>

GameScene::GameScene() : map(1.0f), player(2.0f, 2.0f, 0.8f, map) {}

GameScene::~GameScene() {
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
			worldTransformBlock = nullptr; // 二重解放防止
		}
	}
}


void GameScene::Initialize() {
	// ブロックのモデルを先に読み込む（順番変更OK）
	modelBlock_ = Model::CreateFromOBJ("cube", true);

	// マップのブロックを生成
	GenerateBlocks();
}

void GameScene::Update() {
	// キーボード入力を取得
	if (GetAsyncKeyState('W') & 0x8000)
		player.Move(0, -0.1f);
	if (GetAsyncKeyState('S') & 0x8000)
		player.Move(0, 0.1f);
	if (GetAsyncKeyState('A') & 0x8000)
		player.Move(-0.1f, 0);
	if (GetAsyncKeyState('D') & 0x8000)
		player.Move(0.1f, 0);

	auto pos = player.GetPosition();
	std::cout << "Player Position: (" << pos.x << ", " << pos.z << ")\n";

	// 各ブロックのワールド変換情報を更新
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (worldTransformBlock) {
				worldTransformBlock->UpdateMatrix(true);
			}
		}
	}
}

void GameScene::Draw() {
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (worldTransformBlock) {
				modelBlock_->Draw(*worldTransformBlock, viewProjection_);
			}
		}
	}
}

void GameScene::GenerateBlocks() {
	// `map.GetWidth()` と `map.GetHeight()` を使う
	uint32_t mapWidth = map.GetWidth();
	uint32_t mapHeight = map.GetHeight();

	// 配列をマップサイズに合わせる
	worldTransformBlocks_.resize(mapHeight);
	for (uint32_t i = 0; i < mapHeight; i++) {
		worldTransformBlocks_[i].resize(mapWidth);
	}

	// キューブの生成
	for (uint32_t i = 0; i < mapHeight; i++) {
		for (uint32_t j = 0; j < mapWidth; j++) {
			if (map.GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				// 新しいブロックのワールド変換データを作成
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				// 位置をマップのインデックスから取得
				worldTransform->translation_ = map.GetMapChipPositionByIndex(j, i);

				// 配列に登録
				worldTransformBlocks_[i][j] = worldTransform;
			} else {
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}
}
