#include "GameScene.h"
#include "MapChipField.h"
#include "TextureManager.h"
#include "imgui.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete modelBlock_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	// マップチップフィールドの開放
	delete mapChipFiled_;

	delete debugCamera_;

	// 天球
	delete SkySphere_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクション生成
	viewProjection_.Initialize();

	mapChipFiled_ = new MapChipField;
	mapChipFiled_->LoadMapChipCsv("Resources/Stage01.csv");

	// 表示ブロックの生成
	GenerateBlocks();

	// ブロックのモデルを読み込む
	modelBlock_ = Model::CreateFromOBJ("cube", true);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 天球の生成
	modelSkySphere_ = Model::CreateFromOBJ("SkySphere", true);
	SkySphere_ = new SkySphere();
	SkySphere_->Initialize(modelSkySphere_, &viewProjection_);

	//ビュープロジェクションの初期化
	viewProjection_.farZ = 700;
	viewProjection_.Initialize();
}

void GameScene::Update() {

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;

			//// 平行移動行列
			// Matrix4x4 result = {
			//     1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, worldTransformBlock->translation_.x, worldTransformBlock->translation_.y,
			//     worldTransformBlock->translation_.z, 1.0f};

			// Matrix4x4 matWorld = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			//// 平行移動だけ代入
			// worldTransformBlock->matWorld_ = matWorld;

			//// 定数バッファに転送する
			// worldTransformBlock->TransferMatrix();

			worldTransformBlock->UpdateMatrix();
		}
	}

	debugCamera_->Update();

	//天球の更新
	SkySphere_->Update();

#ifdef _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}

#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

	ImGui::Begin("DebugCamera");

	ImGui::DragFloat3("viewProjection", &viewProjection_.translation_.x, 1.0f);

	ImGui::End();
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

	//天球の描画
	SkySphere_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
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

	// 要素数
	const uint32_t kNumBlockVirtical = mapChipFiled_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipFiled_->GetNumBlockHorizontal();
	// ブロック1個分の横幅
	// const float kBlockWidth = 2.0f;
	// const float kBlockHeight = 2.0f;
	// 要素数を変更する
	// 配列を設定
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		// １列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipFiled_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipFiled_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
