#include "GameScene.h"
#include <iostream>
#include <windows.h>

GameScene::GameScene() : map("Stage01.csv"), player(2.0f, 2.0f, 0.8f, map) {}

GameScene::~GameScene() {
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto*& worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
			worldTransformBlock = nullptr;
		}
	}

	for (uint32_t i = 0; i < mapHeight; i++) {
		worldTransformBlocks_[i].resize(mapWidth, nullptr);
	}
}

void GameScene::Initialize() {
	// ブロックのモデルを先に読み込む（順番変更OK）
	modelBlock_ = Model::CreateFromOBJ("cube", true);

	// マップのブロックを生成
	GenerateBlocks();
}

void GameScene::Update() {
	Vector3 nextPos = player.GetPosition();

	if (GetAsyncKeyState('W') & 0x8000)
		nextPos.z -= 0.1f;
	if (GetAsyncKeyState('S') & 0x8000)
		nextPos.z += 0.1f;
	if (GetAsyncKeyState('A') & 0x8000)
		nextPos.x -= 0.1f;
	if (GetAsyncKeyState('D') & 0x8000)
		nextPos.x += 0.1f;

	// マップの範囲内に収まっているか確認
	uint32_t mapX = static_cast<uint32_t>(nextPos.x);
	uint32_t mapZ = static_cast<uint32_t>(nextPos.z);

	if (mapX < map.GetWidth() && mapZ < map.GetHeight()) {
		if (map.GetMapChipTypeByIndex(mapX, mapZ) != MapChipType::kBlock) {
			player.SetPosition(nextPos); // 壁でない場合のみ移動
		}
	}

	std::cout << "Player Position: (" << nextPos.x << ", " << nextPos.z << ")\n";

	// ブロックのワールド変換情報を更新
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (worldTransformBlock) {
				worldTransformBlock->UpdateMatrix(true);
			}
		}
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 描画するブロックをループで描画
	if (modelBlock_) {
		for (auto& worldTransformBlockLine : worldTransformBlocks_) {
			for (auto& worldTransformBlock : worldTransformBlockLine) {
				if (worldTransformBlock) {
					modelBlock_->Draw(*worldTransformBlock, viewProjection_);
				}
			}
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {

	// 配列をマップサイズに合わせる
	worldTransformBlocks_.resize(map.GetHeight()); // mapHeight → map.GetHeight()
	for (uint32_t i = 0; i < map.GetHeight(); i++) {
		worldTransformBlocks_[i].resize(map.GetWidth()); // mapWidth → map.GetWidth()
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
