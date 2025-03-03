#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	//解放処理
	delete player_;
	//delete debugCamera_;
	delete playerCamera_;
}
 
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();//ビュープロジェクションの初期化

	player_ = new Player();//プレイヤーの生成
	player_->Initialize();//プレイヤーの初期化

	//debugCamera_ = new DebugCamera(1280, 720);// デバッグカメラの生成

	playerCamera_ = new PlayerCamera();//プレイヤーのカメラの生成
	playerCamera_->Initialize({0.0f, 50.0f, 0.0f}, {1.62f, 0.0f, 0.0f});//プレイヤーのカメラの初期化

	
	AxisIndicator::GetInstance()->SetVisible(true);// 軸方向表示の表示を有効化
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);// 軸方向表示が表示するビュープロジェクションを指定する（アドレス渡し）
}

void GameScene::Update() { 

	//debugCamera_->Update();
	playerCamera_->Update();
	viewProjection_.matView = playerCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = playerCamera_->GetViewProjection().matProjection;

	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	player_->Update();//プレイヤーの更新処理
}

void GameScene::Draw() {

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

	player_->Draw(viewProjection_);//プレイヤーの描画

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
