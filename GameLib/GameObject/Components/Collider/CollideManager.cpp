#include "pch.h"
#include "CollideManager.h"

#include "../../GameObject.h"

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

			// リジッドボディの有無をチェック
			RigidBody* aRigid = colA->GetOwn()->GetComponent<RigidBody>();
			RigidBody* bRigid = colB->GetOwn()->GetComponent<RigidBody>();

			if (!aRigid && !bRigid) continue;

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

			HitInfomation hit;

			// 詳細情報を取得するかどうか
			bool hasTrigger = colA->IsTrigger() || colB->IsTrigger();

			bool AisStatic = !aRigid || (aRigid && aRigid->IsStatic());
			bool BisStatic = !bRigid || (bRigid && bRigid->IsStatic());

			bool isStatic = AisStatic && BisStatic;

			// 衝突判定
			if(CheckHit(colA, colB, (!hasTrigger && !isStatic) ? &hit : nullptr))
			{
				// 移動が発生する状態の場合
				if (!hasTrigger && !isStatic)
				{
					// 動く割合の算出
					float ratioA = 0, ratioB = 0;

					// どちらかがStaticな場合
					if (AisStatic) {
						ratioA = 0;   // Aは動かない
						ratioB = 1;   // Bが100%押し戻される
					}
					else if (BisStatic) {
						ratioA = -1;  // Aが100%押し戻される
						ratioB = 0;   // Bは動かない
					}
					// どちらも移動する場合
					else
					{
						// 双方の質量を取得
						float massA = aRigid->GetMass(), massB = bRigid->GetMass();

						// 質量の比率で移動量を決定
						ratioA = -massB / (massA + massB);
						ratioB =  massA / (massA + massB);
					}

					// 位置の補正
					DirectX::SimpleMath::Vector3 corrVecA = ratioA * hit.hitDir * hit.hitLen;
					DirectX::SimpleMath::Vector3 corrVecB = ratioB * hit.hitDir * hit.hitLen;

					colA->GetOwn()->GetComponent<Transform>()->AddWorldPosition(corrVecA);
					colB->GetOwn()->GetComponent<Transform>()->AddWorldPosition(corrVecB);
				}

				// 双方の衝突応答を呼び出す
				colA->GetOwn()->BaseOnCollision(colB);
				colB->GetOwn()->BaseOnCollision(colA);
			}
		}
	}
}
