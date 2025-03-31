#include "Clear.h"
#include "TextureManager.h"
Clear::Clear() {}

Clear::~Clear() { delete sprite; }

void Clear::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();

	textureHandle_ = TextureManager::Load("Resources/GameClear/Clear.png");

	sprite = Sprite::Create(textureHandle_, {0, 0});

	//// 音
	// music = audio_->LoadWave("BGM/GameClear.wav");

	// audio_->PauseWave(music);

	//// 音声再生
	// playMusic = audio_->PlayWave(music, true);
}

void Clear::Update() {
	if (Input::GetInstance()->ReleseKey(DIK_SPACE)) {
		finished_ = true;
		audio_->StopWave(playMusic);
	}

	// タイマーを加算
	timer_ += 1.0f / 60.0f;

	// 行列を更新
}

void Clear::Draw() {
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