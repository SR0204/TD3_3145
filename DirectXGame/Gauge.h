#ifndef GAUGE_H
#define GAUGE_H
#include "Sprite.h"

// ゲージを管理するクラス
class Gauge {
private:
	int value;          // 現在のゲージ値
	int direction;      // ゲージの増減方向（1: 増加, -1: 減少）
	int max_value;      // ゲージの最大値
	int speed;          // ゲージの変動速度
	bool isPaused;      // ゲージの動作を一時停止するフラグ
	Sprite* gaugeSprite_; // ← Sprite をメンバとして保持
public:
	Gauge(int maxValue = 100); // コンストラクタ
	~Gauge();
	void update();              // ゲージを更新（増減処理）
	void setPaused(bool pause); // ゲージの一時停止・再開

	int getValue() const;  // ゲージの現在値を取得
	float getRate() const; // ゲージの割合（0.0〜1.0）を取得

	void Draw(int x, int y);
};

#endif