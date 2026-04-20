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
{

}

// デストラクタ
Scene::~Scene()
{

}

// シーンの変更
void Scene::ChangeScene(const std::string& nextSceneName, std::unique_ptr<SceneTransition> inTrans, std::unique_ptr<SceneTransition> outTrans)
{
	m_pSceneManager->RequestSceneChange(nextSceneName, std::move(inTrans), std::move(outTrans));
}
