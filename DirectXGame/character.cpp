#include "character.h"

// コンストラクタ（HPの初期値を設定）
Character::Character(int initialHp) : hp(initialHp) {}

// ダメージを受ける処理
void Character::takeDamage(int damage, bool isDefending, float defenseRate) {
	if (isDefending) {
		damage = static_cast<int>(damage * (1.0f - defenseRate)); // 防御時にダメージ軽減
	}
	hp -= damage;
	if (hp < 0) {
		hp = 0; // HPが0未満にならないようにする
	}
}

// 現在のHPを取得
int Character::getHp() const { return hp; }

// キャラクターが生きているか判定
bool Character::isAlive() const { return hp > 0; }