#include "game_manager.h"
#include <cmath>
#include <iostream>
// コンストラクタ（ゲームの初期化）
GameManager::GameManager() : gauge(100), player(100), enemy(100) {}
// 毎フレームの更新処理（ゲージの更新）
void GameManager::update() { gauge.update(); }

// ゲージの描画（コンソール出力）
void GameManager::draw() { std::cout << "\rゲージ: " << gauge.getValue() << "  " << std::flush; }

// 攻撃処理（ゲージの値に応じてダメージ決定）
void GameManager::attack() {
	int damage = static_cast<int>(round(10 * (0.5 + gauge.getRate())));
	enemy.takeDamage(damage, false, 0.0f);
	std::cout << "\nプレイヤーの攻撃！ ダメージ: " << damage << " 残りHP: " << enemy.getHp() << std::endl;
}

// ダメージ処理（敵の攻撃を受ける）
void GameManager::takeDamage(int damage, bool isDefending) {
	player.takeDamage(damage, isDefending, gauge.getRate()); // ゲージの割合を渡す
}

// プレイヤーのHPを取得
int GameManager::getPlayerHp() const { return player.getHp(); }

// ゲージの値を取得
int GameManager::getGaugeValue() const { return gauge.getValue(); }

// ゲームオーバー判定（プレイヤーが死亡したか）
bool GameManager::isGameOver() const { return !player.isAlive(); }

// 攻撃処理（攻撃力を指定）
void GameManager::attack(float attackPower) {
	int damage = static_cast<int>(round(10 * attackPower));
	enemy.takeDamage(damage, false, 0.0f);
	std::cout << "\nプレイヤーの攻撃！ ダメージ: " << damage << " 残りHP: " << enemy.getHp() << std::endl;
}