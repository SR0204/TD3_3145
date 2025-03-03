#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <cassert>
#include "ImGui.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	//解放処理
	delete player_;
	delete playerCamera_;
	delete overHeadCamera_;
	delete skydome_;
}
 
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();//ビュープロジェクションの初期化

	player_ = new Player();	//プレイヤーの生成
	player_->Initialize();	//プレイヤーの初期化

	playerCamera_ = new PlayerCamera();									//プレイヤーのカメラの生成
	playerCamera_->Initialize({0.0f, 0.0f, 1.5f}, {0.0f, 0.0f, 0.0f});	//プレイヤーのカメラの初期化
	playerCamera_->SetParent(&player_->GetWorldTransform());		//プレイヤーとカメラの親子関係を結ぶ
	
	overHeadCamera_ = new OverHeadCamera();	//俯瞰カメラの生成
	overHeadCamera_->Initialize();			//俯瞰カメラの初期化

	skydome_ = new Skydome();	//天球の生成
	skydome_->Initialize();		//天球の初期化

	isOverHeadCameraActive_ = false;//俯瞰カメラのアクティブ

	AxisIndicator::GetInstance()->SetVisible(true);							// 軸方向表示の表示を有効化
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);// 軸方向表示が表示するビュープロジェクションを指定する（アドレス渡し）
}

void GameScene::Update() { 
	if (isOverHeadCameraActive_ == false){
		//プレイヤーのカメラの更新
		playerCamera_->Update();
		//ビュープロジェクションにプレイヤーのカメラを登録する
		viewProjection_.matView = playerCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = playerCamera_->GetViewProjection().matProjection;
	}
	else if (isOverHeadCameraActive_ == true){
		//
		overHeadCamera_->Update();
		//
		viewProjection_.matView = overHeadCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = overHeadCamera_->GetViewProjection().matProjection;
	}

	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();
	//プレイヤーの更新処理
	player_->Update();
	//天球の更新処理
	skydome_->Update();
	// ImGuiで値を表示
	ImGui::Begin("Camera");
	if (ImGui::Button("OverHeadCamera")) {
		isOverHeadCameraActive_ = true;
	}
	if (ImGui::Button("PlayerCamera")) {
		isOverHeadCameraActive_ = false;
	}
	ImGui::End();
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
	skydome_->Draw(viewProjection_);//天球の描画

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
