#include "pch.h"
#include "CollideManager.h"

#include "../../GameObject/GameObject.h"

/// <summary>
/// 全てのコライダーの衝突判定を行う関数
/// </summary>
void CollideManager::CheckHitAll()
{
	// 総当たり
	for (size_t i = 0; i < m_colliders.size(); i++)
	{
		BaseCollider* colA = m_colliders[i];

		// コライダーのアクティブ確認
		if (!colA->IsActive()) continue;

		for (size_t j = i + 1; j < m_colliders.size(); j++) 
		{
			BaseCollider* colB = m_colliders[j];

			// コライダーのアクティブ確認
			if (!colB->IsActive()) continue;

			// レイヤーによる衝突判定をするかどうかを管理

			// 条件が存在していれば
			if (m_layer.find(std::make_pair(colA->GetLayerName(), colB->GetLayerName())) != m_layer.end())
			{
				// 指定した2つのレイヤーが衝突しない設定ならスキップ
				if (!m_layer[std::make_pair(colA->GetLayerName(), colB->GetLayerName())]) continue;
			}

			// コライダーの所有者が同じならスキップ
			if (colA->GetOwn() == colB->GetOwn()) continue;

			// コライダーが持つAABBが衝突していなければスキップ
			if (!CheckAABB(colA->GetBoundingBox(), colB->GetBoundingBox())) continue;

			// 衝突判定
			if(CheckHit(colA, colB))
			{
				// 双方の衝突応答を呼び出す
				colA->GetOwn()->OnCollision(colB);
				colB->GetOwn()->OnCollision(colA);
			}
		}
	}
}
