#include "gauge.h"
#include <Sprite.h>
#include <TextureManager.h>
#include <cstdlib> // ランダム値を取得するために使用

// コンストラクタ（ゲージの初期値を設定）
Gauge::Gauge(int maxValue) : value(maxValue / 2), direction((rand() % 2) ? 1 : -1), max_value(maxValue), speed(2), isPaused(false) {

	gaugeSprite_ = new Sprite();
	gaugeSprite_->Initialize();
	gaugeSprite_->SetTextureHandle(TextureManager::Load("./Resources/Gauge/gauge.png"));
	//gaugeSprite_->SetAnchorPoint({0.0f, 0.0f}); // 左上基準で表示
}

Gauge::~Gauge() { delete gaugeSprite_; }

// ゲージの増減処理
void Gauge::update() {
	if (isPaused)
		return; // 停止中なら更新しない

	value += direction * speed; // 速度を考慮した増減
	if (value >= max_value || value <= 0) {
		direction *= -1; // 増減方向を反転
	}
}

// ゲージを一時停止・再開する
void Gauge::setPaused(bool pause) { isPaused = pause; }

// 現在のゲージの値を取得
int Gauge::getValue() const { return value; }

// ゲージの割合（0.0〜1.0）を取得（攻撃力や防御軽減に使用）
float Gauge::getRate() const { return static_cast<float>(value) / max_value; }

void Gauge::Draw(int x, int y) {
	float rate = getRate();
	float maxWidth = 200.0f;
	float height = 20.0f;
	float width = maxWidth * rate;

	gaugeSprite_->SetPosition({float(x), float(y)});
	gaugeSprite_->SetSize({width, height});
	gaugeSprite_->Draw();
}
