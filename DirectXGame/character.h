#ifndef CHARACTER_H
#define CHARACTER_H

// キャラクター（プレイヤー・敵）の管理クラス
class Character {
private:
	int hp; // キャラクターのHP（体力）

public:
	Character() : hp(100) {}                                          // デフォルトコンストラクタを追加
	Character(int initialHp);                                         // コンストラクタ
	void takeDamage(int damage, bool isDefending, float defenseRate); // ダメージを受ける処理
	int getHp() const;                                                // 現在のHPを取得
	bool isAlive() const;                                             // キャラクターが生きているか判定
};

#endif