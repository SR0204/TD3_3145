#include "Time.h"
//#include "imgui.h"


// using namespace KamataEngine;

// 初期化
void Time::Initialize() {
	// 現在時間の保存
	startTime = std::chrono::steady_clock::now(); // 開始時間を設定
}

// 更新
void Time::Update() {
	// elapsedTime で経過時間を秒単位で計算して、時間切れなら "Time Over!" をコンソールに出す。
	auto now = std::chrono::steady_clock::now();
	float elapsedTime = std::chrono::duration<float>(now - startTime).count();

	if (elapsedTime >= limitTime) {
		std::cout << "Time Over!" << std::endl;
	}
}

// 描画
void Time::Draw() {
	// 残り時間をリアルタイム表示しつつ、時間切れなら "Time's up!" も表示する。
	auto now = std::chrono::steady_clock::now();
	float elapsedTime = std::chrono::duration<float>(now - startTime).count();
	float remainingTime = limitTime - elapsedTime;

	if (remainingTime < 0) {
		remainingTime = 0;
	}

	// ImGuiで時間を描画
	/*ImGui::Begin("Time");
	ImGui::Text("Remaining Time: %.2f sec", remainingTime);
	if (IsTimeOver()) {
		ImGui::Text("Time's up!");
	}
	ImGui::End();*/
}

bool Time::IsTimeOver() const {
	// 時間切れ判定関数。const 付きなので Time の状態を変えず、安全に呼び出せる。
	auto now = std::chrono::steady_clock::now();
	return std::chrono::duration<float>(now - startTime).count() >= limitTime;
}
