#include "Over.h"
#include "TextureManager.h"
#include<Xinput.h>

Over::Over() {}

Over::~Over() { delete sprite; }

void Over::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();

	textureHandle_ = TextureManager::Load("GameOver/GameOver.png");

	sprite = Sprite::Create(textureHandle_, {0, 0});

	//// 音
	// music = audio_->LoadWave("BGM/GameOver.wav");

	// audio_->PauseWave(music);

	//// 音声再生
	// playMusic = audio_->PlayWave(music, true);
}

void Over::Update() {
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
}

void Over::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	///
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	sprite->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}