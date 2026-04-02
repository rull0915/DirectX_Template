//--------------------------------------------------------------------------------------
// File: SceneManager.h
//
//--------------------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Transition/Transitions.h"

// 前方参照
class Scene;

// 遷移状態
enum class TransCondition
{
	None,
	In,
	Out,
};

// シーンマネージャークラス
class SceneManager
{

private:

	// エイリアス宣言
	using SceneCollection = std::unordered_map<std::string, std::unique_ptr<Scene>>;

	// シーン群
	SceneCollection m_scenes;

	// 実行中のシーン
	Scene* m_pCurrentScene;

	// 要求されたシーン
	Scene* m_pRequestedScene;

	// Inの演出
	std::unique_ptr<SceneTransition> m_inTrans;
	// Out
	std::unique_ptr<SceneTransition> m_outTrans;

	// 移行状態
	TransCondition m_condition;

	// 実行中のシーン名
	std::string m_nowScene;
	std::string m_saveScene;

public:

	// コンストラクタ
	SceneManager();

	// デストラクタ
	~SceneManager();

	// シーンの登録
	void RegisterScene(const std::string& sceneName, std::unique_ptr<Scene> scene);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	void TransitionRender();

	// 開始シーンの設定
	void SetStartScene(const std::string& startSceneName);

	// シーンの変更
	void RequestSceneChange(const std::string& requestSceneName, std::unique_ptr<SceneTransition> inTrans, std::unique_ptr<SceneTransition> outTrans);

	// 現在実行中のシーン名を返す関数
	const std::string& GetSceneName() const { return m_nowScene; };

private:

	// シーンの変更
	void ChangeScene();
};
