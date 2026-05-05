#include "GamePlayScene.h"
#include "../Game.h"

// コンストラクタ
GamePlayScene::GamePlayScene(Game* pGame)
	: Scene(pGame->GetSceneManager())
	, m_pGame{ pGame }
{
}

GamePlayScene::~GamePlayScene()
{
}

// 初期化関数
void GamePlayScene::Initialize()
{
	// オブジェクトの追加

	// カメラ
	auto camera = Generate<GameObject>();

	auto cameraComponent = camera->AddComponent<CameraComponent>(Screen::WIDTH, Screen::HEIGHT);

	// メインカメラの設定
	GetCamera().SetMainCamera(cameraComponent);

	camera->GetComponent<Transform>()->SetLocalPosition({ 0, 0, 5 });

	// キューブ

	auto obj = Generate<GameObject>();

	// コンポーネントの追加
	obj->AddComponent<BoxCollider>();	// コライダー

	auto render = obj->AddComponent<ModelComponent>();	// 描画
	render->SetModel("Template_Cube");	// モデルをセット

	// 注視点を設定
	cameraComponent->SetUseTarget(true);
	cameraComponent->SetTargetPoint(obj->GetComponent<Transform>()->GetWorldPosition());
}

// 更新関数 
void GamePlayScene::Update(float elapsedTime)
{
}

// 描画関数
void GamePlayScene::Render(Renderer& renderer)
{
}

// 終了関数
void GamePlayScene::Finalize()
{
}