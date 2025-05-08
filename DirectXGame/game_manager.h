#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "character.h"
#include "gauge.h"

// ゲームの管理クラス（プレイヤー・敵・戦闘処理を統括）
class GameManager {
private:
	Gauge gauge;      // ゲージ（攻撃や防御の決定に使用）
	Character player; // プレイヤー
	Character enemy;  // 敵

public:
	GameManager();                                 // コンストラクタ
	void update();                                 // 毎フレームの更新処理
	void draw();                                   // ゲージの描画
	void attack();                                 // 攻撃処理
	void takeDamage(int damage, bool isDefending); // ダメージ処理
	int getPlayerHp() const;                       // // プレイヤーのHPを取得
	int getGaugeValue() const;                     // ゲージの値を取得
	bool isGameOver() const;                       // ゲーム終了判定
	void attack(float attackPower);                // 攻撃処理（攻撃力を指定）
};

#endif