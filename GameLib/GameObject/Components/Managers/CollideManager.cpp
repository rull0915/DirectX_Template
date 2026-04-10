#include "pch.h"
#include "CollideManager.h"

#include "../../GameObject.h"

using namespace DirectX;

void CollideManager::AddReserved()
{
	for (auto& collide : m_reserves)
	{
		m_colliders.push_back(collide);

		collide->UpdateCache();

		// ツリー登録用オブジェクト生成
		ObjectForTree* pOFT = new ObjectForTree();
		pOFT->m_pObject = collide;
		m_treeObjects.push_back(pOFT);

		// ツリーへ登録
		m_tree.AddObject(pOFT);
	}

	m_reserves.clear();
}

void CollideManager::MoveAllColliderOnTree()
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
		}
	}
}

void CollideManager::MakeCollisionList()
{
	// 衝突リストの作成
	m_colCount = m_tree.GetAllCollisionList(m_collideList);
	m_colCount /= 2;
}

/// <summary>
/// 全てのコライダーの衝突判定を行う関数
/// </summary>
void CollideManager::CheckHitAll()
{
	// 衝突リストをループ
	for (int i = 0; i < m_colCount; ++i)
	{
		BaseCollider* colA = m_collideList.at(i * 2);
		BaseCollider* colB = m_collideList.at(i * 2 + 1);

		// コライダーのアクティブ確認
		if (!colA->IsActive() || !colB->IsActive()) continue;

		// リジッドボディの有無をチェック
		RigidBody* aRigid = colA->GetOwn()->GetComponent<RigidBody>();
		RigidBody* bRigid = colB->GetOwn()->GetComponent<RigidBody>();

		// どちらも持っていなければ判定スキップ
		if (!aRigid && !bRigid) continue;

		// レイヤーによる衝突判定をするかどうかを管理

		// 指定した2つのレイヤーが衝突しない設定ならスキップ
		int aLayer = colA->GetLayer(), bLayer = colB->GetLayer();

		if (!(aLayer < 0 || bLayer < 0) && m_layer[aLayer][bLayer]) continue;

		// コライダーの所有者が同じならスキップ
		if (colA->GetOwn() == colB->GetOwn()) continue;

		// コライダーが持つAABBが衝突していなければスキップ
		if (!CheckAABB(colA->GetBoundingBox(), colB->GetBoundingBox())) continue;

		// 衝突情報を取得する変数を用意
		HitInfomation hit;

		// 詳細情報を取得するかどうか
		bool hasTrigger = colA->IsTrigger() || colB->IsTrigger();

		// 各オブジェクトが動かないかどうか
		bool AisStatic = aRigid == nullptr || (aRigid && aRigid->IsStatic());
		bool BisStatic = bRigid == nullptr || (bRigid && bRigid->IsStatic());

		// 衝突時に位置の変動が起こるかどうか
		bool isStatic = AisStatic && BisStatic;

		// 衝突判定
		if (CheckHit(colA, colB, (!hasTrigger && !isStatic) ? &hit : nullptr))
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
					ratioB = massA / (massA + massB);
				}

				// ----- 位置の補正 ----- //
				SimpleMath::Vector3 corrVecA = ratioA * hit.hitDir * hit.hitLen;
				SimpleMath::Vector3 corrVecB = ratioB * hit.hitDir * hit.hitLen;

				colA->GetOwn()->GetComponent<Transform>()->AddWorldPosition(corrVecA);
				colB->GetOwn()->GetComponent<Transform>()->AddWorldPosition(corrVecB);

				// ----- 速度の補正 ----- //

				// --- 衝突法線方向 --- // 

				// 双方の速度を取得
				SimpleMath::Vector3 aVel = (aRigid ? aRigid->GetVelocity() : SimpleMath::Vector3::Zero);
				SimpleMath::Vector3 bVel = (bRigid ? bRigid->GetVelocity() : SimpleMath::Vector3::Zero);

				// 相対速度の計算
				SimpleMath::Vector3 relativeVel = bVel - aVel;

				// 衝突法線方向の速度成分を計算
				float hitDirVel = relativeVel.Dot(hit.hitDir);

				// ぶつかっていれば
				if (hitDirVel < 0)
				{
					// 反発係数の小さいほうを適用
					float aRes = (!AisStatic ? aRigid->GetRestitution() : 1), bRes = (!BisStatic ? bRigid->GetRestitution() : 1);
					float e = aRes < bRes ? aRes : bRes;

					// インパルスの算出
					float aInvMass = !AisStatic ? aRigid->GetInvMass() : 0, bInvMass = (!BisStatic ? bRigid->GetInvMass() : 0);
					float j = (-(1 + e) * hitDirVel) / (aInvMass + bInvMass);	// 打ち消しと反発を同時に行うために-(1 + e)

					// 摩擦係数を取得
					float aFric = (aRigid ? aRigid->GetFriction() : 0), bFric = (bRigid ? bRigid->GetFriction() : 0);
					float mu = (aFric + bFric) / 2;

					// --- 接線方向の処理 --- //

					// 接線ベクトルの作成
					// 相対速度から「法線方向の成分」を抜き出す
					SimpleMath::Vector3 normalVel = hitDirVel * hit.hitDir;

					// 全体の相対速度から法線成分を引く
					SimpleMath::Vector3 tangentVel = relativeVel - normalVel;

					// 正規化して接線とする
					float tangentSpeed = tangentVel.Length();
					SimpleMath::Vector3 tangential = SimpleMath::Vector3::Zero;

					if (tangentSpeed > 0.0001f) {
						tangential = tangentVel / tangentSpeed;
					}

					// 接線方向の速度をゼロにするインパルスを計算
					float j_tangent = -(relativeVel.Dot(tangential)) / (aInvMass + bInvMass);

					// 摩擦の限界値を計算 垂直抗力 j に 摩擦係数 mu を掛ける
					float maxFriction = mu * abs(j);

					// ブレーキの強さを限界値内に収める
					float actualFrictionImpulse = MyMath::Clamp(j_tangent, -maxFriction, maxFriction);

					// 接線方向のインパルス
					SimpleMath::Vector3 tangentImpulse = actualFrictionImpulse * tangential;
					// 法線方向のインパルス
					SimpleMath::Vector3 normalImpulse = j * hit.hitDir;

					// 双方の速度を変更
					if (aRigid) aRigid->SetVelocity(aVel - aInvMass * (tangentImpulse + normalImpulse));
					if (bRigid) bRigid->SetVelocity(bVel + bInvMass * (tangentImpulse + normalImpulse));
				}
			}

			// 双方の衝突応答を呼び出す
			colA->GetOwn()->BaseOnCollision(colB);
			colB->GetOwn()->BaseOnCollision(colA);
		}
	}

	//// 総当たり
	//for (size_t i = 0; i < m_colliders.size(); i++)
	//{
	//	BaseCollider* colA = m_colliders[i];

	//	// コライダーのアクティブ確認
	//	if (!colA->IsActive()) continue;

	//	for (size_t j = i + 1; j < m_colliders.size(); j++) 
	//	{
	//		BaseCollider* colB = m_colliders[j];

	//		// コライダーのアクティブ確認
	//		if (!colB->IsActive()) continue;

	//		// リジッドボディの有無をチェック
	//		RigidBody* aRigid = colA->GetOwn()->GetComponent<RigidBody>();
	//		RigidBody* bRigid = colB->GetOwn()->GetComponent<RigidBody>();

	//		// どちらも持っていなければ判定スキップ
	//		if (!aRigid && !bRigid) continue;

	//		// レイヤーによる衝突判定をするかどうかを管理

	//		// 条件が存在していれば
	//		if (m_layer.find(std::make_pair(colA->GetLayerName(), colB->GetLayerName())) != m_layer.end())
	//		{
	//			// 指定した2つのレイヤーが衝突しない設定ならスキップ
	//			if (!m_layer[std::make_pair(colA->GetLayerName(), colB->GetLayerName())]) continue;
	//		}

	//		// コライダーの所有者が同じならスキップ
	//		if (colA->GetOwn() == colB->GetOwn()) continue;

	//		// コライダーが持つAABBが衝突していなければスキップ
	//		if (!CheckAABB(colA->GetBoundingBox(), colB->GetBoundingBox())) continue;

	//		// 衝突情報を取得する変数を用意
	//		HitInfomation hit;

	//		// 詳細情報を取得するかどうか
	//		bool hasTrigger = colA->IsTrigger() || colB->IsTrigger();

	//		// 各オブジェクトが動かないかどうか
	//		bool AisStatic = aRigid == nullptr || (aRigid && aRigid->IsStatic());
	//		bool BisStatic = bRigid == nullptr || (bRigid && bRigid->IsStatic());

	//		// 衝突時に位置の変動が起こるかどうか
	//		bool isStatic = AisStatic && BisStatic;

	//		// 衝突判定
	//		if (CheckHit(colA, colB, (!hasTrigger && !isStatic) ? &hit : nullptr))
	//		{
	//			// 移動が発生する状態の場合
	//			if (!hasTrigger && !isStatic)
	//			{
	//				// 動く割合の算出
	//				float ratioA = 0, ratioB = 0;

	//				// どちらかがStaticな場合
	//				if (AisStatic) {
	//					ratioA = 0;   // Aは動かない
	//					ratioB = 1;   // Bが100%押し戻される
	//				}
	//				else if (BisStatic) {
	//					ratioA = -1;  // Aが100%押し戻される
	//					ratioB = 0;   // Bは動かない
	//				}
	//				// どちらも移動する場合
	//				else
	//				{
	//					// 双方の質量を取得
	//					float massA = aRigid->GetMass(), massB = bRigid->GetMass();

	//					// 質量の比率で移動量を決定
	//					ratioA = -massB / (massA + massB);
	//					ratioB = massA / (massA + massB);
	//				}

	//				// ----- 位置の補正 ----- //
	//				SimpleMath::Vector3 corrVecA = ratioA * hit.hitDir * hit.hitLen;
	//				SimpleMath::Vector3 corrVecB = ratioB * hit.hitDir * hit.hitLen;

	//				colA->GetOwn()->GetComponent<Transform>()->AddWorldPosition(corrVecA);
	//				colB->GetOwn()->GetComponent<Transform>()->AddWorldPosition(corrVecB);

	//				// ----- 速度の補正 ----- //

	//				// --- 衝突法線方向 --- // 

	//				// 双方の速度を取得
	//				SimpleMath::Vector3 aVel = (aRigid ? aRigid->GetVelocity() : SimpleMath::Vector3::Zero);
	//				SimpleMath::Vector3 bVel = (bRigid ? bRigid->GetVelocity() : SimpleMath::Vector3::Zero);

	//				// 相対速度の計算
	//				SimpleMath::Vector3 relativeVel = bVel - aVel;

	//				// 衝突法線方向の速度成分を計算
	//				float hitDirVel = relativeVel.Dot(hit.hitDir);

	//				// ぶつかっていれば
	//				if (hitDirVel < 0)
	//				{
	//					// 反発係数の小さいほうを適用
	//					float aRes = (!AisStatic ? aRigid->GetRestitution() : 1), bRes = (!BisStatic ? bRigid->GetRestitution() : 1);
	//					float e = aRes < bRes ? aRes : bRes;

	//					// インパルスの算出
	//					float aInvMass = !AisStatic ? aRigid->GetInvMass() : 0, bInvMass = (!BisStatic ? bRigid->GetInvMass() : 0);
	//					float j = (-(1 + e) * hitDirVel) / (aInvMass + bInvMass);	// 打ち消しと反発を同時に行うために-(1 + e)

	//					// 摩擦係数を取得
	//					float aFric = (aRigid ? aRigid->GetFriction() : 0), bFric = (bRigid ? bRigid->GetFriction() : 0);
	//					float mu = (aFric + bFric) / 2;

	//					// --- 接線方向の処理 --- //

	//					// 接線ベクトルの作成
	//					// 相対速度から「法線方向の成分」を抜き出す
	//					SimpleMath::Vector3 normalVel = hitDirVel * hit.hitDir;

	//					// 全体の相対速度から法線成分を引く
	//					SimpleMath::Vector3 tangentVel = relativeVel - normalVel;

	//					// 正規化して接線とする
	//					float tangentSpeed = tangentVel.Length();
	//					SimpleMath::Vector3 tangential = SimpleMath::Vector3::Zero;

	//					if (tangentSpeed > 0.0001f) {
	//						tangential = tangentVel / tangentSpeed;
	//					}

	//					// 接線方向の速度をゼロにするインパルスを計算
	//					float j_tangent = -(relativeVel.Dot(tangential)) / (aInvMass + bInvMass);

	//					// 摩擦の限界値を計算 垂直抗力 j に 摩擦係数 mu を掛ける
	//					float maxFriction = mu * abs(j);

	//					// ブレーキの強さを限界値内に収める
	//					float actualFrictionImpulse = MyMath::Clamp(j_tangent, -maxFriction, maxFriction);

	//					// 接線方向のインパルス
	//				 	SimpleMath::Vector3 tangentImpulse = actualFrictionImpulse * tangential;
	//					// 法線方向のインパルス
	//					SimpleMath::Vector3 normalImpulse = j * hit.hitDir;

	//					// 双方の速度を変更
	//					if (aRigid) aRigid->SetVelocity(aVel - aInvMass * (tangentImpulse + normalImpulse));
	//					if (bRigid) bRigid->SetVelocity(bVel + bInvMass * (tangentImpulse + normalImpulse));
	//				}
	//			}

	//			// 双方の衝突応答を呼び出す
	//			colA->GetOwn()->BaseOnCollision(colB);
	//			colB->GetOwn()->BaseOnCollision(colA);
	//		}
	//	}
	//}
}
