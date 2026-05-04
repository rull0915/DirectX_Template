//====================================================//
// ファイル名   : Scene.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/02/06
//
// 概要 : Sceneの基底クラス
//
// 更新履歴 :
// 2026/02/06 新規作成
// 2026/05/04 コンポーネントのマネージャーを所持するように変更
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Transition/Transitions.h"
#include "GameLib/Common/Renderer/Renderer.h"

// 各マネージャー
#include "GameLib/GameObject/Managers/3DManagers/PhysicsManager.h"
#include "GameLib/GameObject/Managers/2DManagers/PhysicsManager2D.h"
#include "GameLib/GameObject/Managers/Renderer/RendererManager.h"
#include "GameLib/GameObject/Managers/CameraManager.h"
#include "GameLib/GameObject/Managers/ObjectManager.h"
#include "GameLib/GameObject/Managers/System/CollideEventSystem.h"

//====================================================//
// 前方宣言
//====================================================//

class SceneManager;

//====================================================//
// クラス宣言
//====================================================//
class Scene
{
private:

	// シーンマネージャーへのポインタ
	SceneManager* m_pSceneManager;

	// 各マネージャーの実体
	PhysicsManager m_physicsManager;		// 3D物理
	PhysicsManager2D m_physicsManager2D;	// 2D物理
	CollideEventSystem m_colEvent;	// 衝突時のイベント

	CameraManager m_cameraManager;		// カメラ
	RendererManager m_rendererManager;	// 描画
	ObjectManager m_objectManager;		// オブジェクト

public:

	// コンストラクタ
	Scene(SceneManager* pSceneManager);

	// デストラクタ
	virtual ~Scene();

	// 初期化処理
	virtual void Initialize() = 0;

	// 更新処理
	void BaseUpdate(float elapsedTime);
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	void BaseRender(Renderer& renderer);
	virtual void Render(Renderer& renderer) = 0;

	// 終了処理
	virtual void Finalize() = 0;

public:
	// 各マネージャーのゲッター
	PhysicsManager& GetPhysics() { return m_physicsManager; }
	PhysicsManager2D& GetPhysics2D() { return m_physicsManager2D; }

	CameraManager& GetCamera() { return m_cameraManager; }
	RendererManager& GetRenderer() { return m_rendererManager; }
	ObjectManager& GetObject(){ return m_objectManager; }

	// オブジェクトを生成する関数
	template<typename T, typename... Args>
	T* Generate(Args&&... args);

	// コンポーネントを登録する関数
	void RegisterComponent(BaseComponent* component);

	// コンポーネントの登録を解除する関数
	void UnRegsiterComponent(BaseComponent* component);

protected:

	// シーンの変更
	void ChangeScene(const std::string& nextSceneName, std::unique_ptr<SceneTransition> inTrans, std::unique_ptr<SceneTransition> outTrans);
};

template<typename T, typename ...Args>
inline T* Scene::Generate(Args && ...args)
{
	// GameObject派生クラスであれば
	if constexpr (std::is_base_of_v<GameObject, T>)
	{
		// ポインタを作成
		T* pObj = new T(std::forward<Args>(args)...);

		// オブジェクトのシーンに自身を設定
		pObj->SetScene(this);

		// オブジェクト管理クラスに追加
		m_objectManager.AddObject(pObj);

		// 作成したポインタを返す
		return pObj;
	}

	return nullptr;
}

/// <summary>
/// コンポーネントを登録する関数
/// </summary>
/// <typeparam name="T">コンポーネントの型名</typeparam>
inline void Scene::RegisterComponent(BaseComponent* component)
{
	int id = component->GetID();

	switch (id & 0xFF00)
	{
	case MAIN_CAMERA:
		m_cameraManager.SetMainCamera(static_cast<CameraComponent*>(component));
		break;
	case MAIN_RIGIDBODY:
		m_physicsManager.AddRigidBody(static_cast<RigidBody*>(component));
		break;
	case MAIN_RIGIDBODY_2D:
		m_physicsManager2D.AddRigidBody(static_cast<RigidBody2D*>(component));
		break;
	case MAIN_COLLIDER:
		m_physicsManager.GetCollideManager().AddCollide(static_cast<BaseCollider*>(component));
		break;
	case MAIN_COLLIDER_2D:
		m_physicsManager2D.GetCollideManager().AddCollide(static_cast<BaseCollider2D*>(component));
		break;
	case MAIN_RENDERER:
		m_rendererManager.AddRenderer(static_cast<BaseRenderer*>(component));
		break;
	default:
		break;
	}
}

inline void Scene::UnRegsiterComponent(BaseComponent* component)
{
	int id = component->GetID();

	switch (id & 0xFF00)
	{
	case MAIN_RIGIDBODY:
		m_physicsManager.RemoveRigidBody(static_cast<RigidBody*>(component));
		break;
	case MAIN_RIGIDBODY_2D:
		m_physicsManager2D.RemoveRigidBody(static_cast<RigidBody2D*>(component));
		break;
	case MAIN_COLLIDER:
		m_physicsManager.GetCollideManager().RemoveCollide(static_cast<BaseCollider*>(component));
		break;
	case MAIN_COLLIDER_2D:
		m_physicsManager2D.GetCollideManager().RemoveCollide(static_cast<BaseCollider2D*>(component));
		break;
	case MAIN_RENDERER:
		m_rendererManager.RemoveRenderer(static_cast<BaseRenderer*>(component));
		break;
	default:
		break;
	}
}
