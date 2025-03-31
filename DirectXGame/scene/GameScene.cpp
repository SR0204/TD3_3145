#include "GameScene.h"
#include "AxisIndicator.h"
#include "MapChipField.h"
#include "TextureManager.h"
// #include "imgui.h"
#include "Player.h"
#include <cassert>
#include <fstream>
#include <iostream>

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete modelBlock_;
	delete modelSkySphere_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	// マップチップフィールドの開放
	delete mapChipFiled_;

	// 天球
	delete SkySphere_;

	delete player_;
	delete playerCamera_;
	delete overHeadCamera_;

	// 敵
	delete model_;

	delete enemy_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクション生成
	viewProjection_.Initialize();

	mapChipFiled_ = new MapChipField;
	mapChipFiled_->LoadMapChipCsv("Resources/Stage01.csv");

	// 音
	// music = audio_->LoadWave("sound/n003.wav");

	// audio_->PauseWave(music);

	// 音声再生
	// playMusic = audio_->PlayWave(music, true);

	// 表示ブロックの生成
	GenerateBlocks();
	GenerateClearBlocks();

	// ブロックのモデルを読み込む
	modelBlock_ = Model::CreateFromOBJ("cube", true);

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipFiled_->GetMapChipPositionByIndex(14, 3);
	playerPosition.y = 2;

	player_ = new Player();              // プレイヤーの生成
	player_->Initialize(playerPosition); // プレイヤーの初期化
	player_->SetMapChipField(mapChipFiled_);

	playerCamera_ = new PlayerCamera();                                // プレイヤーのカメラの生成
	playerCamera_->Initialize({0.0f, 0.0f, 1.5f}, {0.0f, 0.0f, 0.0f}); // プレイヤーのカメラの初期化
	playerCamera_->SetParent(&player_->GetWorldTransform());           // プレイヤーとカメラの親子関係を結ぶ

	overHeadCamera_ = new OverHeadCamera(); // 俯瞰カメラの生成
	overHeadCamera_->Initialize();          // 俯瞰カメラの初期化

	isOverHeadCameraActive_ = false; // 俯瞰カメラのアクティブ

	AxisIndicator::GetInstance()->SetVisible(true);                          // 軸方向表示の表示を有効化
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_); // 軸方向表示が表示するビュープロジェクションを指定する（アドレス渡し）

	// 天球の生成
	modelSkySphere_ = Model::CreateFromOBJ("SkySphere", true);
	SkySphere_ = new SkySphere();
	SkySphere_->Initialize(modelSkySphere_, &viewProjection_);

	// Enemy
	enemyTextureHandle_ = TextureManager::Load("uvChecker.png");
	LoadEnemyPopData();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 700;
	viewProjection_.Initialize();

	// 制限時間初期化
	StartTimer_ = 500;
}

void GameScene::Update() {

	if (isOverHeadCameraActive_ == false) {
		// プレイヤーのカメラの更新
		playerCamera_->Update();
		// ビュープロジェクションにプレイヤーのカメラを登録する
		viewProjection_.matView = playerCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = playerCamera_->GetViewProjection().matProjection;
	} else if (isOverHeadCameraActive_ == true) {

		overHeadCamera_->Update();

		viewProjection_.matView = overHeadCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = overHeadCamera_->GetViewProjection().matProjection;
	}

	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	// ImGuiで値を表示
	/*ImGui::Begin("Camera");
	if (ImGui::Button("OverHeadCamera")) {
	    isOverHeadCameraActive_ = true;
	}
	if (ImGui::Button("PlayerCamera")) {
	    isOverHeadCameraActive_ = false;
	}
	ImGui::End();*/

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatrixBlock();
		}
	}

	// プレイヤーの更新処理
	player_->Update();

	// 天球の更新
	SkySphere_->Update();

	// Enemy更新
	UpDateEnemyPopCommands();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	StartTimer_--;

	// シーン切り替え
	Player::CollisionMapInfo collisionMapInfo;
	if (player_->CheckCollisionWithClearBlock(collisionMapInfo) == true) {
		// クリアブロックに当たった場合
		isClear_ = true;
		isFinished = true;
		// audio_->StopWave(playMusic);
	}

	if (player_->GetHp() <= 0) {
		isFinished = true;

		audio_->StopWave(playMusic);
	}

	if (StartTimer_ == 0) {
		isFinished = true;

		// audio_->StopWave(playMusic);
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

	player_->Draw(viewProjection_); // プレイヤーの描画

	// 天球の描画
	SkySphere_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	// Enemy描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
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
	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipFiled_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;

				// XZ軸に合わせて位置を設定
				worldTransformBlocks_[i][j]->translation_ = mapChipFiled_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::GenerateClearBlocks() {

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipFiled_->GetMapChipTypeByIndex(j, i) == MapChipType::kClear) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;

				// XZ軸に合わせて位置を設定
				worldTransformBlocks_[i][j]->translation_ = mapChipFiled_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

#pragma region 敵発生関連関数
void GameScene::SpawnEnemy(Vector3 position) {
	Enemy* newEnemy = new Enemy();

	newEnemy->SetGameScene(this); // GameScene をセット

	newEnemy->Initialize(model_, enemyTextureHandle_, &viewProjection_, position);

	/*newEnemy->SetPlayer(player_);*/

	enemies_.push_back(newEnemy);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpDateEnemyPopCommands() {
	// 待機処理
	if (waitFlag) {
		waitTimer--;
		if (waitTimer <= 0) {
			// 待機完了
			waitFlag = false;
		}
		return;
	}

	// １行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// １行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;

		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z));
		}

		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			waitFlag = true;
			waitTimer = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}
#pragma endregion
