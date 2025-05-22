#pragma once

#include <cstdint> // uint32_t用

class Sprite; // ← これを追加！

class BattleScene {
public:
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished();

	void Finalize();

private:
	Sprite* bgSprite_ = nullptr;
	uint32_t bgTex_ = 0;

	bool isFinished_ = false;
};
