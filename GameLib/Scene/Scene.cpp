#include "Scene.h"
#include "SceneManager.h"

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
