//====================================================//
// ファイル名  : Scene.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/02/17
//
// 概要        :  シーンの基底クラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "Scene.h"
#include "SceneManager.h"

//====================================================//
// 関数の実体宣言
//====================================================//

// コンストラクタ
Scene::Scene(SceneManager* pSceneManager)
	: m_pSceneManager(pSceneManager)
	, m_physicsManager{}
	, m_physicsManager2D{}
	, m_colEvent{}
	, m_cameraManager{}
	, m_rendererManager{}
	, m_objectManager{}
{
}

// デストラクタ
Scene::~Scene()
{

}

void Scene::BaseUpdate(float elapsedTime)
{
	// 派生クラスの更新
	Update(elapsedTime);

	// 各オブジェクトの更新
	m_objectManager.Update(elapsedTime);

	// リジッドボディの更新
	m_physicsManager.Update(elapsedTime);

	// 2Dリジッドボディの更新
	m_physicsManager2D.Update(elapsedTime);

	// カメラの更新
	m_cameraManager.Update();

	// 衝突判定後の値の更新
	m_objectManager.AllReflectCache();

	// 衝突後関数の呼び出し
	m_colEvent.CallCollideFunctions(m_physicsManager.GetHitList());

	// 描画管理クラスの更新
	m_rendererManager.Update();
}

/// <summary>
/// 基底描画関数
/// </summary>
/// <param name="renderer"></param>
void Scene::BaseRender(Renderer& renderer)
{
	// メインカメラの行列を適用
	renderer.SetView(m_cameraManager.GetView());
	renderer.SetProjection(m_cameraManager.GetProj());

	// 描画管理クラスの描画処理
	m_rendererManager.DrawAll(renderer);

	// オブジェクトの描画処理
	m_objectManager.Render(renderer);

	// 派生クラスの描画処理
	renderer.SetView(DirectX::SimpleMath::Matrix::Identity);
	renderer.SetProjection(DirectX::SimpleMath::Matrix::Identity);

	Render(renderer);
}

// シーンの変更
void Scene::ChangeScene(const std::string& nextSceneName, std::unique_ptr<SceneTransition> inTrans, std::unique_ptr<SceneTransition> outTrans)
{
	m_pSceneManager->RequestSceneChange(nextSceneName, std::move(inTrans), std::move(outTrans));
}
