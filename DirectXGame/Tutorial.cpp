#include "Tutorial.h"
#include "GameScene.h"
#include <numbers>

Tutorial::Tutorial() {}

Tutorial::~Tutorial() { delete modelFont_; }

void Tutorial::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	modelFont_ = Model::CreateFromOBJ("Tutorial");

	viewProjection_.Initialize();

	worldTransformFont_.Initialize();

	dxCommon_ = DirectXCommon::GetInstance();

	worldTransformFont_.translation_.x = -1;
	worldTransformFont_.rotation_.y = -1.5;
	worldTransformFont_.scale_ = {7, 5, 5};

	// 音
	music = audio_->LoadWave("BGM/Tutorial.mp3");

	audio_->PauseWave(music);

	// 音声再生
	playMusic = audio_->PlayWave(music, true);
}

void Tutorial::Update() {
	Input* input = Input::GetInstance();

	// XInputの状態を取得
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD dwResult = XInputGetState(0, &state);

	// スペースキー or コントローラーのAボタンで画面遷移
	if (input->ReleseKey(DIK_SPACE) || (dwResult == ERROR_SUCCESS && (state.Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		finished_ = true;
		audio_->StopWave(playMusic);
	}

	// タイマーを加算
	timer_ += 1.0f / 60.0f;

	// 行列を更新
	worldTransformFont_.UpdateMatrixBlock();
}

void Tutorial::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	// ここに３Dオブジェクトの描画処理を追加できる
	modelFont_->Draw(worldTransformFont_, viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
}
