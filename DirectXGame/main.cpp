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
	gameScene = new GameScene();
	gameScene->Initialize();

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
		//gameScene->Update();
		
		// titleScene->Update();

		// シーン切り替え
		ChangeScene();
		// 現在シーン更新
		UpdateScene();

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
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTutorial;
			// 旧シーンの開放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			tutorialScene = new Tutorial;
			tutorialScene->Initialize();
		}
		break;

	case Scene::kTutorial:
		if (tutorialScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの開放
			delete tutorialScene;
			tutorialScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			if (gameScene->IsClear() == true) {
				// シーン変更
				scene = Scene::kClear;
				// 旧シーンの開放
				delete gameScene;
				gameScene = nullptr;
				// 新シーンの生成と初期化
				clearScene_ = new Clear;
				clearScene_->Initialize();
			} else {
				// シーン変更
				scene = Scene::kOver;
				// 旧シーンの開放
				delete gameScene;
				gameScene = nullptr;
				// 新シーンの生成と初期化
				overScene_ = new Over;
				overScene_->Initialize();
			}
		}
		break;
	case Scene::kClear:
		if (clearScene_->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの開放
			delete clearScene_;
			clearScene_ = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	case Scene::kOver:
		if (overScene_->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの開放
			delete overScene_;
			overScene_ = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kTutorial:
		tutorialScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		gameScene->DrawTimeUI(); // Time::Draw() を呼び出す専用の関数
		break;
	case Scene::kClear:
		clearScene_->Update();
		break;
	case Scene::kOver:
		overScene_->Update();
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kTutorial:
		tutorialScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:
		clearScene_->Draw();
		break;
	case Scene::kOver:
		overScene_->Draw();
		break;
	}
}
