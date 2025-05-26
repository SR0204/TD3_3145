#include "BattleScene.h"
#include "TextureManager.h"
#include <DirectXCommon.h>
#include <cstdio>

const int ENEMY_ATTACK_INTERVAL = 90; // 敵の攻撃間隔（1.5秒）

void BattleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 背景テクスチャ読み込み
	bgTex_ = TextureManager::Load("./Resources/background.png");

	// 背景スプライト初期化
	if (!bgSprite_) {
		bgSprite_ = new Sprite();
		bgSprite_->Initialize();
	}
	bgSprite_->SetTextureHandle(bgTex_);
	bgSprite_->SetAnchorPoint({0.5f, 0.5f});
	bgSprite_->SetPosition({640.0f, 360.0f});
	bgSprite_->SetSize({1280.0f, 720.0f});
	bgSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// プレイヤーと敵のHP初期化（ここ重要）
	player = Character(100); // HP100のキャラを作成
	enemy = Character(1);

	// ゲージの初期化（毎回作り直す）
	if (attackGauge) {
		delete attackGauge;
	}
	attackGauge = new Gauge(100); // 最大値100

	if (defenseGauge) {
		delete defenseGauge;
	}
	defenseGauge = new Gauge(100); // 最大値100

	// 状態初期化
	enemyAttackTimer = 0;
	isDefending = false;
	isFinished_ = false;
	result_ = BattleResult::PlayerLose;
}

void BattleScene::Update() {
	attackGauge->update();
	defenseGauge->update();

	// 攻撃キー
	if (input_->TriggerKey(DIK_SPACE)) {
		float attackPower = attackGauge->getRate();
		int damage = static_cast<int>(attackPower * 20);
		if (damage <= 0) {
			damage = 1; // 最低1ダメージ保証
		}
		enemy.takeDamage(damage, false, 0.0f);

		printf("プレイヤーが攻撃！ ダメージ: %d, 敵HP: %d\n", damage, enemy.getHp());
	}

	// 防御キー
	if (input_->PushKey(DIK_D)) {
		isDefending = true;
	} else {
		isDefending = false;
	}

	// 敵の攻撃処理
	enemyAttackTimer++;
	if (enemyAttackTimer >= ENEMY_ATTACK_INTERVAL) {
		enemyAttackTimer = 0;

		int enemyDamage = 15;
		float defenseRate = defenseGauge->getRate();
		player.takeDamage(enemyDamage, isDefending, defenseRate);

		int actualDamage = isDefending ? static_cast<int>(enemyDamage * (1.0f - defenseRate)) : enemyDamage;
		printf("敵が攻撃！ 受けたダメージ: %d, プレイヤーHP: %d\n", actualDamage, player.getHp());
	}

	// 勝敗判定（例: HPが0で終了）
	if (enemy.getHp() <= 0) {
		result_ = BattleResult::PlayerWin;
		isFinished_ = true;
	} else if (player.getHp() <= 0) {
		result_ = BattleResult::PlayerLose;
		isFinished_ = true;
	}
}

void BattleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	Sprite::PreDraw(commandList);

	if (bgSprite_) {
		bgSprite_->Draw();
	}

	if (attackGauge)
		attackGauge->Draw(100, 600);
	if (defenseGauge)
		defenseGauge->Draw(100, 650);


	Sprite::PostDraw();
}

bool BattleScene::IsFinished() { return isFinished_; }

BattleScene::BattleResult BattleScene::GetResult() const {
	return result_; // 勝敗を返す（Update中で設定済み）
}

void BattleScene::Finalize() {
	if (bgSprite_) {
		delete bgSprite_;
		bgSprite_ = nullptr;
	}

	if (attackGauge) {
		delete attackGauge;
		attackGauge = nullptr;
	}

	if (defenseGauge) {
		delete defenseGauge;
		defenseGauge = nullptr;
	}
}