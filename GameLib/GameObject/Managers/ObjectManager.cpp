//====================================================//
// ファイル名  : ObjectManager.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/07
//
// 概要        :  GameObjectを管理するクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "ObjectManager.h"

#include "2DManagers/Collider/CollideManager2D.h"
#include "2DManagers/PhysicsManager2D.h"

#include "3DManagers/Collider/CollideManager.h"
#include "3DManagers/PhysicsManager.h"

#include "CameraManager.h"

#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
ObjectManager::ObjectManager()
{
}

/// <summary>
/// デストラクタ
/// </summary>
ObjectManager::~ObjectManager()
{
}

/// <summary>
/// 更新関数
/// </summary>
/// <param name="elapsedTime">1fの経過時間</param>
void ObjectManager::Update(float elapsedTime)
{
	// 予約されているオブジェクトを追加
	AddReservedObject();

	// リジッドボディの更新
	PhysicsManager::Instance().Update(elapsedTime);

	// 2Dリジッドボディの更新
	PhysicsManager2D::Instance().Update(elapsedTime);

	// 全オブジェクトの更新関数呼び出し
	for (auto& object : m_objects)
	{
		// 基底クラスの更新処理
		object->BaseUpdate(elapsedTime);

		// 派生クラスの更新処理
		object->Update(elapsedTime);
	}

	// カメラの更新
	CameraManager::Instance().Update();

	{
		// 3Dコライダーの更新
		
		// 木構造の更新
		CollideManager::Instance().MoveAllColliderOnTree();
		// 衝突判定
		CollideManager::Instance().CheckHitAll();

		// 2Dコライダーの更新

		// 木構造の更新
		CollideManager2D::Instance().MoveAllColliderOnTree();
		// 衝突判定
		CollideManager2D::Instance().CheckHitAll();
	}

	// 衝突判定後の値の更新
	for (auto& object : m_objects)
	{
		object->GetComponent<Transform>()->ReflectCache();	// Transform
	}

	// 死亡オブジェクトの削除
	RemoveDeadObject();
}

/// <summary>
/// 描画関数
/// </summary>
void ObjectManager::Render()
{
	// メインカメラの行列を適用
	MyRenderer::SetView(CameraManager::Instance().GetView());
	MyRenderer::SetProjection(CameraManager::Instance().GetProj());

	// 全オブジェクトの描画
	for (auto& object : m_objects)
	{
		// 基底クラスの描画処理
		object->BaseRender();

		// 派生クラスの描画処理
		object->Render();
	}
}

/// <summary>
/// 終了処理
/// </summary>
void ObjectManager::Finalize()
{
	AllDestroy();
	RemoveDeadObject();
}

void ObjectManager::AllDestroy()
{
	// 管理リストを全て調べる
	for (auto& obj : m_objects)
	{
		obj->Destroy();
	}
}

/// <summary>
/// 予約済みオブジェクトを全て追加する関数
/// </summary>
void ObjectManager::AddReservedObject()
{
	// 予約リストを全て調べる
	for (auto& p : m_reservations)
	{
		// 管理リストへ追加
		m_objects.push_back(std::unique_ptr<GameObject>(p));

		// 初期化関数の呼び出し
		p->Initialize();
	}

	// 予約リストをリセット
	m_reservations.clear();
}

void ObjectManager::RemoveDeadObject()
{
	// 管理リストを全て調べる
	for (int i = 0; i < m_objects.size(); i++)
	{
		GameObject* obj = m_objects[i].get();

		// オブジェクトの死亡フラグがオンなら
		if (obj->IsDead())
		{
			obj->BaseFinalize();

			// 終了関数の呼び出し
			obj->Finalize();

			// コライダーをマネージャーから削除
			std::vector<BaseCollider*> cols;
			obj->GetComponents<BaseCollider>(cols);
			for (auto& col : cols) CollideManager::Instance().RemoveCollide(col);

			// リジッドボディをマネージャーから削除
			PhysicsManager::Instance().RemoveRigidBody(obj->GetComponent<RigidBody>());

			// リストから削除
			m_objects.erase(m_objects.begin() + i);

			i--;
		}
	}
}
