#include "Timer.h"
#include "Sprite.h"
#include <string>

// コンストラクタ：制限時間を設定
Timer::Timer(float limit) : limitTime(limit) {}

void Timer::Initialize() {
	// 現在時間の保存（高精度タイマーの開始）
	QueryPerformanceFrequency(&frequency); // タイマーの周波数を取得
	QueryPerformanceCounter(&startTime);   // 開始時刻を取得
}

void Timer::Update() {
	// 経過時間を秒単位で計算
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	float elapsedTime = static_cast<float>(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;

	if (elapsedTime >= limitTime) {
		std::cout << "Time Over!" << std::endl;
	}
}

void Timer::Draw(int numberTextures[], float x, float y) {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	// 経過時間の計算
	float elapsedTime = static_cast<float>(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
	float remainingTime = limitTime - elapsedTime;
	if (remainingTime < 0)
		remainingTime = 0;

	int displayTime = static_cast<int>(remainingTime);
	std::string timeText = std::to_string(displayTime);

	float offsetX = x;

	// 数字を一つずつ処理
	for (char c : timeText) {
		int digit = c - '0';

		// Spriteを新しく生成
		Sprite* digitSprite = Sprite::Create(
		    numberTextures[digit], // 数字用テクスチャ
		    {offsetX, y}           // 座標
		);

		// スプライトを描画
		digitSprite->Draw();

		offsetX += 64; // 次の数字用にX座標をずらす
	}
}

bool Timer::IsTimeOver() const {
	// 時間切れ判定
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	float elapsedTime = static_cast<float>(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
	return elapsedTime >= limitTime;
}