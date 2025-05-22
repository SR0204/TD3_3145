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
	delete enemy_;

	// 制限時間
	delete timer_;

	// メモリ解放などがあればここに記述
	delete gaugeSprite_; // スプライトのメモリ解放（必要に応じて）
	delete BackgroundSprite_;
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

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 700;
	viewProjection_.Initialize();

	// 制限時間の初期化
	// 数字テクスチャのロード
	for (int i = 0; i <= 9; i++) {
		std::string path = "Numbers/" + std::to_string(i) + ".png";
		numberTextures_[i] = TextureManager::Load(path);
	}
	timer_ = new Timer(1000.0f); // 制限時間を変更できるよ
	timer_->Initialize();

	gaugeSprite_ = Sprite::Create(TextureManager::Load("./Resources/Gauge/gauge.png"), {50, 300});
	BackgroundSprite_ = Sprite::Create(TextureManager::Load("uvChecker.png"), {0, 0});

	// 敵のマップ座標指定
	Vector3 enemyPosition = mapChipFiled_->GetMapChipPositionByIndex(5, 3);
	enemyPosition.y = 3;

	// 敵の初期化処理
	model_ = Model::CreateFromOBJ("Ghost");                                    // モデルの生成
	enemyTextureHandle_ = TextureManager::Load("./Resources/Ghost/Ghost.png"); // テクスチャの読み込み

	enemy_ = new Enemy();
	enemy_->Initialize(enemyPosition, model_);
	enemy_->SetMapChipField(mapChipFiled_);
	enemy_->SetPlayerPosition(player_->GetWorldTransform().translation_);

	shouldStartBattle_ = false;
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

	// 敵の更新処理
	enemy_->Update();

	Vector3 playerPos = player_->GetPosition();
	Vector3 enemyPos = enemy_->GetWorldTransform().translation_;

	float dx = playerPos.x - enemyPos.x;
	float dz = playerPos.z - enemyPos.z;
	float distance = std::sqrt(dx * dx + dz * dz);

	if (distance < 1.0f) {
		shouldStartBattle_ = true;
	}

	// 天球の更新
	SkySphere_->Update();

	// 制限時間の更新
	timer_->Update();

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

	if (shouldStartBattle_ == true) {
		BackgroundSprite_->Draw(); // 背景スプライトの描画
	}

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

	// 敵の描画
	enemy_->Draw(&viewProjection_);

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
	// if (shouldStartBattle_ == true) {

	//	gaugeSprite_->Draw(); // スプライトの描画
	//}

	//  3Dオブジェクト描画後処理
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

// bool GameScene::ShouldStartBattle() const { return isBattleTriggered_; }

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
