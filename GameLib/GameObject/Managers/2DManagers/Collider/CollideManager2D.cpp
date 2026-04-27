//====================================================//
// ファイル名  : SampleClass.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要        :  衝突判定管理クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "CollideManager2D.h"

#include "GameLib/GameObject/GameObject.h"
#include "GameLib/GameMath/GameMath.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

void CollideManager2D::AddReserved()
{
	for (auto& collide : m_reserves)
	{
		m_colliders.push_back(collide);

		// ツリー登録用オブジェクト生成
		ObjectForTree2D* pOFT = new ObjectForTree2D();
		pOFT->m_pObject = collide;
		m_treeObjects.push_back(pOFT);

		// ツリーへ登録
		m_tree.AddObject(pOFT);
	}

	m_reserves.clear();
}

void CollideManager2D::UpdateCaches()
{
	// 全てのコライダー
	for (auto& collide : m_colliders)
	{
		// Dirthフラグが立っていればキャッシュを更新
		if(collide->IsDirty()) collide->UpdateCache();
	}
}

void CollideManager2D::MoveAllColliderOnTree()
{
	// 予約済みコライダーの登録
	AddReserved();

	// 全オブジェクトのチェック
	for (size_t i = 0; i < m_colliders.size(); i++)
	{
		// もし値が変わっていたら
		if (m_colliders[i]->IsDirty())
		{
			// キャッシュの更新
			m_colliders[i]->UpdateCache();
		}
		// もし変更フラグが立っていたら
		if (m_colliders[i]->IsChanged())
		{
			// 木構造を更新

			// 一度リストから外れる
			m_treeObjects[i]->Remove();

			// 再登録
			m_tree.AddObject(m_treeObjects[i]);

			m_colliders[i]->ResetChangeFrag();
		}
	}
}

/// <summary>
/// 全てのコライダーの衝突判定を行う関数
/// </summary>
void CollideManager2D::CheckHitAll(std::vector<HitContact2D>& contacts)
{
	// 条件のラムダ式
	auto condition = [&](BaseCollider2D* a, BaseCollider2D* b)
		{
			// コライダーのアクティブ確認
			if (!a->m_isActive || !b->m_isActive) return false;

			// コライダーの所有者が同じならスキップ
			if (a->m_own == b->m_own) return false;

			// コライダーが持つAABBが衝突していなければスキップ
			AABB2D aabb1 = a->m_boundingBox, aabb2 = b->m_boundingBox;

			if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) return false;
			if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) return false;

			return true;
		};

	// 衝突判定を行うラムダ式
	auto action = [&](BaseCollider2D* a, BaseCollider2D* b)
		{
			HitContact2D contact;

			bool hit = CheckHitPair(a, b, contact);

			if (hit)
			{
				contacts.push_back(contact);
			}
		};

	// 衝突リストの初期化
	contacts.clear();

	// 衝突判定
	m_tree.CheckAllCollisionList(condition, action);
}

bool CollideManager2D::CheckHitPair(BaseCollider2D* colA, BaseCollider2D* colB, HitContact2D& contact)
{
	// リジッドボディの有無をチェック
	RigidBody2D* aRigid = colA->GetOwn()->GetComponent<RigidBody2D>();
	RigidBody2D* bRigid = colB->GetOwn()->GetComponent<RigidBody2D>();

	// どちらも持っていなければ判定スキップ
	if (!aRigid && !bRigid) return false;

	bool aSleep = (!aRigid || aRigid->IsSleep());
	bool bSleep = (!bRigid || bRigid->IsSleep());

	// どちらもスリープであれば判定スキップ
//	if (aSleep && bSleep) return false;

	// 衝突情報を取得する変数を用意
	HitInfomation2D hit;

	// 詳細情報を取得するかどうか
	bool hasTrigger = colA->IsTrigger() || colB->IsTrigger();

	// 各オブジェクトが動かないかどうか
	bool AisStatic = !aRigid || (aRigid && aRigid->IsStatic());
	bool BisStatic = !bRigid || (bRigid && bRigid->IsStatic());

	// 両方staticなら判定スキップ
	if (AisStatic && BisStatic) return false;

	// 衝突判定
	if (CheckHit2D(colA, colB, !hasTrigger ? &hit : nullptr))
	{
		contact.a = colA->GetOwn(), contact.b = colB->GetOwn();

		contact.aCol = colA, contact.bCol = colB;

		contact.aRigid = aRigid, contact.bRigid = bRigid;

		contact.aIsStatic = AisStatic, contact.bIsStatic = BisStatic;

		contact.isTrigger = hasTrigger;

		if (!hasTrigger)
		{
			contact.normal = hit.hitDir;
			contact.penetration = hit.hitLen;
		}

		return true;
	}

	return false;
}
