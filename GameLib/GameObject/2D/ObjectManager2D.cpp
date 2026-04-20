//====================================================//
// ファイル名  : ObjectManager2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要        :  GameObjectを管理するクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "ObjectManager2D.h"

#include "Managers/CollideManager2D.h"
#include "Managers/PhysicsManager2D.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
ObjectManager2D::ObjectManager2D()
{
}

/// <summary>
/// デストラクタ
/// </summary>
ObjectManager2D::~ObjectManager2D()
{
}

/// <summary>
/// 更新関数
/// </summary>
/// <param name="elapsedTime">1fの経過時間</param>
void ObjectManager2D::Update(float elapsedTime)
{
	// 予約されているオブジェクトを追加
	AddReservedObject();
	
	// オブジェクトのキャッシュ更新
	{
		for (auto& object : m_objects)
		{
			object->GetComponent<Transform2D>()->UpdateCache();	// Transform
		}

		CollideManager2D::Instance().UpdateCaches();		// Collider
	}

	// リジッドボディの更新
	PhysicsManager2D::Instance().Update(elapsedTime);

	// 全オブジェクトの更新関数呼び出し
	for (auto& object : m_objects)
	{
		// 基底クラスの更新処理
		object->BaseUpdate(elapsedTime);

		// 派生クラスの更新処理
		object->Update(elapsedTime);
	}

	// オブジェクトのキャッシュ更新
	{
		for (auto& object : m_objects)
		{
			object->GetComponent<Transform2D>()->UpdateCache();	// Transform
		}
		CollideManager2D::Instance().UpdateCaches();		// Collider
	}

	{
		// 木構造の更新
		CollideManager2D::Instance().MoveAllColliderOnTree();

		// 衝突判定
		CollideManager2D::Instance().CheckHitAll();

		// 値の更新
		for (auto& object : m_objects)
		{
			object->GetComponent<Transform2D>()->ReflectCache();	// Transform
		}
	}

	// 死亡オブジェクトの削除
	RemoveDeadObject();
}

/// <summary>
/// 描画関数
/// </summary>
void ObjectManager2D::Render()
{
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
void ObjectManager2D::Finalize()
{
	AllDestroy();
	RemoveDeadObject();
}

void ObjectManager2D::AllDestroy()
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
void ObjectManager2D::AddReservedObject()
{
	// 予約リストを全て調べる
	for (auto& p : m_reservations)
	{
		// 管理リストへ追加
		m_objects.push_back(std::unique_ptr<GameObject2D>(p));

		// 初期化関数の呼び出し
		p->Initialize();
	}

	// 予約リストをリセット
	m_reservations.clear();
}

void ObjectManager2D::RemoveDeadObject()
{
	// 管理リストを全て調べる
	for (int i = 0; i < m_objects.size(); i++)
	{
		GameObject2D* obj = m_objects[i].get();

		// オブジェクトの死亡フラグがオンなら
		if (obj->IsDead())
		{
			obj->BaseFinalize();

			// 終了関数の呼び出し
			obj->Finalize();

			// コライダーをマネージャーから削除
			std::vector<BaseCollider2D*> cols;
			obj->GetComponents<BaseCollider2D>(cols);
			for (auto& col : cols) CollideManager2D::Instance().RemoveCollide(col);

			// リジッドボディをマネージャーから削除
			PhysicsManager2D::Instance().RemoveRigidBody2D(obj->GetComponent<RigidBody2D>());

			// リストから削除
			m_objects.erase(m_objects.begin() + i);

			i--;
		}
	}
}
