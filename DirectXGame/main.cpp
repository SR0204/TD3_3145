#include "Audio.h"
#include "AxisIndicator.h"
#include "Clear.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "Over.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "Tutorial.h"
#include "WinApp.h"
// #include "gauge.h"
// #include "character.h"

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
Tutorial* tutorialScene = nullptr;
Clear* clearScene_ = nullptr;
Over* overScene_ = nullptr;

// シーン(型)
enum class Scene {
	kUnknown = 0,

	kTitle,
	kTutorial,
	kGame,
	kBattle,
	kClear,
	kOver,
};

// 現在シーン
Scene scene = Scene::kUnknown;

void ChangeScene();
void UpdateScene();
void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	// GameScene* gameScene = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"3145_ダンジョンコード");
	win->SetSizeChangeMode(WinApp::SizeChangeMode::kNone);

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// ゲームシーンの初期化
	/*gameScene = new GameScene();
	gameScene->Initialize();*/

	// タイトル
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// ゲームシーンの毎フレーム処理
		// gameScene->Update();

		// titleScene->Update();

		// 現在シーン更新
		UpdateScene();
		// シーン切り替え
		ChangeScene();

		// 軸表示の更新
		axisIndicator->Update();

		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		// ゲームシーンの描画
		// gameScene->Draw();

		// 現在シーンの描画
		DrawScene();

		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	delete gameScene;
	delete titleScene;
	delete tutorialScene;
	delete clearScene_;
	delete overScene_;

	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene && titleScene->IsFinished()) {
			if (!tutorialScene) {
				tutorialScene = new Tutorial;
				tutorialScene->Initialize();
			}
			delete titleScene;
			titleScene = nullptr;
			scene = Scene::kTutorial;
		}
		break;

	case Scene::kTutorial:
		if (tutorialScene && tutorialScene->IsFinished()) {
			delete tutorialScene;
			tutorialScene = nullptr;

			if (!gameScene) {
				gameScene = new GameScene;
				gameScene->Initialize();
			}

			scene = Scene::kGame;
		}
		break;

	case Scene::kGame:
		if (gameScene) {
			if (gameScene->IsFinished()) {
				if (gameScene->IsClear()) {
					delete gameScene;
					gameScene = nullptr;

					if (!clearScene_) {
						clearScene_ = new Clear;
						clearScene_->Initialize();
					}
					scene = Scene::kClear;
					return; // ← 追加：このフレームでの処理をここで終わらせる

				} else {
					delete gameScene;
					gameScene = nullptr;

					if (!overScene_) {
						overScene_ = new Over;
						overScene_->Initialize();
					}
					scene = Scene::kOver;
				}
			}
		}
		break;

	case Scene::kBattle:

		break;

	case Scene::kClear:
		if (clearScene_ && clearScene_->IsFinished()) {
			delete clearScene_;
			clearScene_ = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();
			scene = Scene::kTitle;
		}
		break;

	case Scene::kOver:
		if (overScene_ && overScene_->IsFinished()) {
			delete overScene_;
			overScene_ = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();
			scene = Scene::kTitle;
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Update();
		break;
	case Scene::kTutorial:
		if (tutorialScene)
			tutorialScene->Update();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Update();
		break;
	case Scene::kBattle:

		break;
	case Scene::kClear:
		if (clearScene_)
			clearScene_->Update();
		break;
	case Scene::kOver:
		if (overScene_)
			overScene_->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene)
			titleScene->Draw();
		break;
	case Scene::kTutorial:
		if (tutorialScene)
			tutorialScene->Draw();
		break;
	case Scene::kGame:
		if (gameScene)
			gameScene->Draw();
		break;
	case Scene::kBattle:

		break;
	case Scene::kClear:
		if (clearScene_)
			clearScene_->Draw();
		break;
	case Scene::kOver:
		if (overScene_)
			overScene_->Draw();
		break;
	}
}
