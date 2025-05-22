#include "BattleScene.h"
#include "Sprite.h"
#include "TextureManager.h"
#include <DirectXCommon.h>

void BattleScene::Initialize() {
	// 初期化処理

	bgTex_ = TextureManager::Load("./Resources/background.png");

	bgSprite_ = new Sprite();
	bgSprite_->Initialize();
	bgSprite_->SetTextureHandle(bgTex_);
	bgSprite_->SetAnchorPoint({0.5f, 0.5f});
	bgSprite_->SetPosition({640.0f, 360.0f}); // 1280x720 の中央

	// 画面サイズに拡大（例: 1280x720のウィンドウを想定）
	bgSprite_->SetSize({1280.0f, 720.0f});
	bgSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
}

void BattleScene::Update() {
	// バトルロジック
	// バトルが終了したら isFinished_ = true にする
}

void BattleScene::Draw() {
	// コマンドリスト取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// Sprite描画の準備
	Sprite::PreDraw(commandList);

	// 背景スプライト描画
	if (bgSprite_) {
		bgSprite_->Draw();
	}

	// Sprite描画終了
	Sprite::PostDraw();
}

bool BattleScene::IsFinished() { return isFinished_; }

void BattleScene::Finalize() {
	delete bgSprite_;
	bgSprite_ = nullptr;
}
