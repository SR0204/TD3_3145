#include "GameScene.h"
#include "AxisIndicator.h"
#include "MapChipField.h"
#include "TextureManager.h"
// #include "imgui.h"
#include "Enemy.h"
#include "Player.h"
#include "Timer.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>

const int ENEMY_ATTACK_INTERVAL = 90; // 敵の攻撃間隔（1.5秒）

GameScene::GameScene()
    : timer_(nullptr), gameManager(), attackGauge(100), defenseGauge(100), player(100), enemy(100)

{
	isClear_ = false;
}

GameScene::~GameScene() {

	delete modelBlock_;
	delete mapClearModel_;
	delete modelSkySphere_;

	// 壁のとかのブロック
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlockList_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	// クリアブロック
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformClearBlockList_) {
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

	delete timer_;

	// メモリ解放などがあればここに記述
	delete gaugeSprite_;      // スプライトのメモリ解放（必要に応じて）
	delete BackgroundSprite_; // スプライトのメモリ解放（必要に応じて）4
	delete EnemySprite_;      // スプライトのメモリ解放（必要に応じて）
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
	music = audio_->LoadWave("BGM/Play.MP3");

	audio_->PauseWave(music);
	// 音声再生
	playMusic = audio_->PlayWave(music, true, 1);

	// 表示ブロックの生成
	GenerateBlocks();
	GenerateClearBlocks();

	// ブロックのモデルを読み込む
	modelBlock_ = Model::CreateFromOBJ("cube", true);
	mapClearModel_ = Model::CreateFromOBJ("Goal", true);

	isClear_ = false;
	isFinished = false;

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipFiled_->GetMapChipPositionByIndex(14, 3);
	playerPosition.y = 2;

	player_ = new Player(this);          // プレイヤーの生成
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
	// enemyTextureHandle_ = TextureManager::Load("uvChecker.png");
	// LoadEnemyPopData();

	// enemy_->Initialize(model_, enemyTextureHandle_, &viewProjection_, Vector3{14.0f, 2.0f, 3.0f});

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 700;
	viewProjection_.Initialize();

	// 制限時間の初期化
	// 数字テクスチャのロード
	for (int i = 0; i <= 9; i++) {
		std::string path = "Numbers/" + std::to_string(i) + ".png";
		numberTextures_[i] = TextureManager::Load(path);
	}
	timer_ = new Timer(120.0f); // 制限時間を変更できるよ
	timer_->Initialize();

	gaugeSprite_ = Sprite::Create(TextureManager::Load("./Resources/Gauge/gauge.png"), {50, 300});
	// BackgroundSprite_ = Sprite::Create(TextureManager::Load("background.png"), {0, 0});
	EnemySprite_ = Sprite::Create(TextureManager::Load("uvChecker.png"), {200, 200});
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
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlockList_) {

		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatrixBlock();
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformClearBlockList_) {

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

	// 制限時間の更新
	timer_->Update();

	CheckEnemyCollision(); // 敵と接触していないかチェック

	// シーン切り替え
	Player::CollisionMapInfo collisionMapInfo;
	if (player_->CheckCollisionWithClearBlock(collisionMapInfo)) {
		if (player_->IsClear()) {
			SetClear();             // 状態セット
			player_->OnGameClear(); // 演出だけ担当
			audio_->StopWave(playMusic);
		}
	}

	if (player_->GetHp() <= 0) {
		isFinished = true;

		audio_->StopWave(playMusic);
	}

	if (timer_->IsTimeOver()) {
		isFinished = true;

		audio_->StopWave(playMusic);
	}

	// ゲージ更新
	attackGauge.update();
	defenseGauge.update();

	if (input_->PushKey(DIK_SPACE)) {
		float attackPower = attackGauge.getRate();       // ゲージの割合を攻撃力として使用
		int damage = static_cast<int>(attackPower * 20); // 20を基準ダメージとする
		enemy.takeDamage(damage, false, 0.0f);           // 敵に攻撃（防御なし）

		// デバッグ用出力
		printf("プレイヤーが攻撃！ ダメージ: %d, 敵HP: %d\n", damage, enemy.getHp());
	} // 修正後のattack関数を呼び出す

	// 防御ボタン（例: Dキー）
	if (input_->PushKey(DIK_D)) {
		isDefending = true;
	} else {
		isDefending = false;
	}
	// 敵の攻撃処理
	enemyAttackTimer++;
	if (enemyAttackTimer >= ENEMY_ATTACK_INTERVAL) {
		enemyAttackTimer = 0;                       // タイマーリセット
		int enemyDamage = 15;                       // 敵の基本攻撃力
		float defenseRate = defenseGauge.getRate(); // 防御ゲージの割合を軽減率に使用
		player.takeDamage(enemyDamage, isDefending, defenseRate);

		// デバッグ用出力
		printf("敵が攻撃！ 受けたダメージ: %d, プレイヤーHP: %d\n", isDefending ? static_cast<int>(enemyDamage * (1.0f - defenseRate)) : enemyDamage, player.getHp());
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

	// BackgroundSprite_->Draw(); // 背景スプライトの描画

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
	for (auto& blockLine : worldTransformBlockList_) {
		for (auto* block : blockLine) {
			if (!block)
				continue;
			modelBlock_->Draw(*block, viewProjection_);
		}
	}

	// クリアブロックの描画
	for (auto& clearBlockLine : worldTransformClearBlockList_) {
		for (auto* clearBlock : clearBlockLine) {
			if (!clearBlock)
				continue;
			mapClearModel_->Draw(*clearBlock, viewProjection_);
		}
	}

	// gaugeSprite_->Draw();      // スプライトの描画

	//EnemySprite_->Draw(); // 敵スプライトの描画
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// タイマー描画（画面右上あたりに表示）
	timer_->Draw(numberTextures_, 1000.0f, 10.0f);

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::SetClear() { isClear_ = true; }

bool GameScene::ShouldStartBattle() const { return isBattleTriggered_; }

void GameScene::CheckEnemyCollision() {
	//Vector3 playerPos = player_->GetPosition();
	//Vector3 playerSize = player_->GetSize(); // 一度だけ定義

	//for (Enemy*enemy : enemies_) {
	//	if (!enemy)
	//		continue;

	//	Vector3 enemyPos = enemy->GetWorldPosition();
	//	Vector3 enemySize = enemy->GetSize();

	//	bool isColliding = playerPos.x < enemyPos.x + enemySize.x && playerPos.x + playerSize.x > enemyPos.x && playerPos.y < enemyPos.y + enemySize.y && playerPos.y + playerSize.y > enemyPos.y &&
	//	                   playerPos.z < enemyPos.z + enemySize.z && playerPos.z + playerSize.z > enemyPos.z;

	//	if (isColliding) {
	//		isBattleTriggered_ = true;
	//		break;
	//	}
	//}
}

// void GameScene::ResetBattleTrigger() { isBattleTriggered_ = false; }

// #pragma region 敵発生関連関数

// void GameScene::SpawnEnemy(Vector3 position) {
//	Enemy* newEnemy = new Enemy();
//
//	newEnemy->SetGameScene(this); // GameScene をセット
//
//	newEnemy->Initialize(model_, enemyTextureHandle_, &viewProjection_, position);
//
//	/*newEnemy->SetPlayer(player_);*/
//
//	enemies_.push_back(newEnemy);
// }
//
// void GameScene::LoadEnemyPopData() {
//	// ファイルを開く
//	std::ifstream file;
//	file.open("Resources/Enemy/enemyPop.csv");
//	assert(file.is_open());
//
//	// ファイルの内容を文字列ストリームにコピー
//	enemyPopCommands << file.rdbuf();
//
//	// ファイルを閉じる
//	file.close();
// }
//
// void GameScene::UpDateEnemyPopCommands() {
//	// 待機処理
//	if (waitFlag) {
//		waitTimer--;
//		if (waitTimer <= 0) {
//			// 待機完了
//			waitFlag = false;
//		}
//		return;
//	}
//
//	// １行分の文字列を入れる変数
//	std::string line;
//
//	// コマンド実行ループ
//	while (getline(enemyPopCommands, line)) {
//		// １行分の文字列をストリームに変換して解析しやすくする
//		std::istringstream line_stream(line);
//
//		std::string word;
//
//		// ,区切りで行の先頭文字列を取得
//		getline(line_stream, word, ',');
//
//		// "//"から始まる行はコメント
//		if (word.find("//") == 0) {
//			// コメント行を飛ばす
//			continue;
//		}
//
//		// POPコマンド
//		if (word.find("POP") == 0) {
//			// x座標
//			getline(line_stream, word, ',');
//			float x = (float)std::atof(word.c_str());
//
//			// y座標
//			getline(line_stream, word, ',');
//			float y = (float)std::atof(word.c_str());
//
//			// z座標
//			getline(line_stream, word, ',');
//			float z = (float)std::atof(word.c_str());
//
//			// 敵を発生させる
//			SpawnEnemy(Vector3(x, y, z));
//		}
//
//		// WAITコマンド
//		else if (word.find("WAIT") == 0) {
//			getline(line_stream, word, ',');
//
//			// 待ち時間
//			int32_t waitTime = atoi(word.c_str());
//
//			// 待機開始
//			waitFlag = true;
//			waitTimer = waitTime;
//
//			// コマンドループを抜ける
//			break;
//		}
//	}
// }
void GameScene::DrawTimeUI() {

	if (timer_) {
		timer_->Draw(numberTextures_, 1000.0f, 10.0f); // TimeクラスのDrawを呼び出す
	}
}
#pragma endregion

void GameScene::GenerateBlocks() {
	worldTransformBlockList_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlockList_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipFiled_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlockList_[i][j] = worldTransform;
				worldTransformBlockList_[i][j]->translation_ = mapChipFiled_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::GenerateClearBlocks() {

	worldTransformClearBlockList_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformClearBlockList_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipFiled_->GetMapChipTypeByIndex(j, i) == MapChipType::kClear) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				Vector3 position = mapChipFiled_->GetMapChipPositionByIndex(j, i);
				position.y = 2.0f; // ← プレイヤーと同じ高さに設定！
				worldTransform->translation_ = position;

				// ブロックの向き
				worldTransform->rotation_ = {0.0f, 90.0f, 0.0f};

				worldTransformClearBlockList_[i][j] = worldTransform;
			}
		}
	}
}
