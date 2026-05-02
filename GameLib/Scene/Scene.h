#pragma once

#include "Transition/Transitions.h"

#include "GameLib/Common/Renderer/Renderer.h"

// 前方宣言
class SceneManager;

// シーンクラス
class Scene
{
private:

	// シーンマネージャーへのポインタ
	SceneManager* m_pSceneManager;

public:

	// コンストラクタ
	Scene(SceneManager* pSceneManager);

	// デストラクタ
	virtual ~Scene();

	// 初期化処理
	virtual void Initialize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render(Renderer& renderer) = 0;

	// 終了処理
	virtual void Finalize() = 0;

protected:

	// シーンの変更
	void ChangeScene(const std::string& nextSceneName, std::unique_ptr<SceneTransition> inTrans, std::unique_ptr<SceneTransition> outTrans);
};
