//====================================================//
// ファイル名  : SceneManager.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/02/17
//
// 概要        :  シーン管理クラス 遷移演出もここで管理しています
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "SceneManager.h"
#include "Scene.h"

//====================================================//
// 関数の実体宣言
//====================================================//

// コンストラクタ
SceneManager::SceneManager()
	: m_scenes{}
	, m_pCurrentScene{ nullptr }
	, m_pRequestedScene{ nullptr }
	, m_inTrans{}
	, m_outTrans{}
	, m_condition{ TransCondition::None }
	, m_nowScene{}
	, m_saveScene{}
{
}

// デストラクタ
SceneManager::~SceneManager()
{
	// 実行中のシーンの終了処理
	if (m_pCurrentScene) m_pCurrentScene->Finalize();
}

// シーンの登録
void SceneManager::RegisterScene(const std::string& sceneName, std::unique_ptr<Scene> scene)
{
	// シーンの名前でシーンを登録
	m_scenes.emplace(sceneName, std::move(scene));
}

// 更新処理
void SceneManager::Update(float elapsedTime)
{
	switch (m_condition)
	{
	case TransCondition::None:

		// シーンの更新
		if (m_pCurrentScene) m_pCurrentScene->Update(elapsedTime);

		break;
	case TransCondition::In:

		if (m_inTrans->Update(elapsedTime))
		{
			// シーンの変更
			if (m_pRequestedScene) ChangeScene();

			m_condition = TransCondition::Out;
			m_inTrans = nullptr;

			m_outTrans->Initialize();
		}

		break;
	case TransCondition::Out:

		if (m_outTrans->Update(elapsedTime))
		{
			m_condition = TransCondition::None;
			m_inTrans = nullptr;
		}

		break;
	default:
		break;
	}
}

// 描画処理
void SceneManager::Render()
{
	// シーンの描画処理
	if (m_pCurrentScene) m_pCurrentScene->Render();
}

void SceneManager::TransitionRender()
{
	switch (m_condition)
	{
	case TransCondition::None:
		break;
	case TransCondition::In:
		m_inTrans->Render();
		break;
	case TransCondition::Out:
		m_outTrans->Render();
		break;
	default:
		break;
	}
}

// 開始シーンの設定
void SceneManager::SetStartScene(const std::string& startSceneName)
{
	RequestSceneChange(startSceneName, std::make_unique<FadeTransition>(0, Transition::Mode::In, 0x000000), std::make_unique<FadeTransition>(1.0f, Transition::Mode::Out, 0x000000));
}

void SceneManager::RequestSceneChange(const std::string& requestSceneName, std::unique_ptr<SceneTransition> inTrans, std::unique_ptr<SceneTransition> outTrans)
{
	// シーンが未登録？
	if (m_scenes.find(requestSceneName) == m_scenes.end())
	{
		OutputDebugString(L"%sは登録されていません。");
		return;
	}

	// 登録されたリクエストシーンを取得
	m_pRequestedScene = m_scenes[requestSceneName].get();

	// Inの遷移を開始
	m_inTrans = std::move(inTrans);
	m_outTrans = std::move(outTrans);

	m_condition = TransCondition::In;
	m_inTrans->Initialize();

	m_saveScene = requestSceneName;
}

// シーンの変更
void SceneManager::ChangeScene()
{
	// シーンのリクエスト要求がない？
	if (!m_pRequestedScene) return;

	// 実行中のシーンの終了処理
	if (m_pCurrentScene) m_pCurrentScene->Finalize();

	// リクエスト要求のシーンへ移行
	m_pCurrentScene = m_pRequestedScene;

	// シーンの初期化処理
	m_pCurrentScene->Initialize();
	m_pCurrentScene->Update(0.f);

	// リクエスト要求をクリア
	m_pRequestedScene = nullptr;

	// 名前の更新
	m_nowScene = m_saveScene;
}
