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
#include "CollideManager.h"

#include "GameLib/GameObject/GameObject.h"
#include "GameLib/GameMath/GameMath.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

void CollideManager::AddReserved()
{
	for (auto& collide : m_reserves)
	{
		m_colliders.push_back(collide);

		// ツリー登録用オブジェクト生成
		ObjectForTree* pOFT = new ObjectForTree();
		pOFT->m_pObject = collide;
		m_treeObjects.push_back(pOFT);

		// ツリーへ登録
		m_tree.AddObject(pOFT);
	}

	m_reserves.clear();
}

void CollideManager::UpdateCaches()
{
	// 全てのコライダー
	for (auto& collide : m_colliders)
	{
		// Dirthフラグが立っていればキャッシュを更新
		if(collide->IsDirty()) collide->UpdateCache();
	}
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

			m_colliders[i]->ResetChangeFrag();
		}
	}
}

/// <summary>
/// 全てのコライダーの衝突判定を行う関数
/// </summary>
void CollideManager::CheckHitAll()
{
	// 条件のラムダ式
	auto condition = [&](BaseCollider* a, BaseCollider* b)
		{
			// コライダーのアクティブ確認
			if (!a->m_isActive || !b->m_isActive) return false;

			// コライダーの所有者が同じならスキップ
			if (a->m_own == b->m_own) return false;

			// コライダーが持つAABBが衝突していなければスキップ
			AABB aabb1 = a->m_boundingBox, aabb2 = b->m_boundingBox;

			if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) return false;
			if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) return false;
			if (aabb1.min.z > aabb2.max.z || aabb2.min.z > aabb1.max.z) return false;

			return true;
		};

	// 衝突判定を行うラムダ式
	auto action = [&](BaseCollider* a, BaseCollider* b)
		{
			CheckHitPair(a, b);
		};

	// 衝突判定
	m_tree.CheckAllCollisionList(condition, action);
}

void CollideManager::CheckHitPair(BaseCollider* colA, BaseCollider* colB)
{
	// 指定した2つのレイヤーが衝突しない設定ならスキップ
	int aLayer = colA->GetLayer(), bLayer = colB->GetLayer();

	if (!(aLayer < 0 || bLayer < 0 || aLayer >= 100 || bLayer >= 100) && m_layer[aLayer][bLayer]) return;

	// リジッドボディの有無をチェック
	RigidBody* aRigid = colA->GetOwn()->GetComponent<RigidBody>();
	RigidBody* bRigid = colB->GetOwn()->GetComponent<RigidBody>();

	// どちらも物理挙動を持っていなければ判定スキップ
	if (!aRigid && !bRigid) return;

	bool aSleep = (!aRigid || aRigid->IsSleep());
	bool bSleep = (!bRigid || bRigid->IsSleep());

	// どちらもスリープであれば判定スキップ
	if (aSleep && bSleep)
	{
		return;
	}

	// 衝突情報を取得する変数を用意
	HitInfomation hit;

	// 詳細情報を取得するかどうか
	bool hasTrigger = colA->IsTrigger() || colB->IsTrigger();

	// 各オブジェクトが動かないかどうか
	bool AisStatic = !aRigid || (aRigid && aRigid->IsStatic());
	bool BisStatic = !bRigid || (bRigid && bRigid->IsStatic());

	// 衝突時に位置の変動が起こるかどうか
	bool isStatic = AisStatic && BisStatic;

	// 衝突判定
	if (CheckHit(colA, colB, (!hasTrigger && !isStatic) ? &hit : nullptr))
	{
		// 移動が発生する状態の場合
		if (!hasTrigger && !isStatic)
		{
			// どちらも起こす
			if (aRigid && aSleep) aRigid->WakeUp();
			if (bRigid && bSleep) bRigid->WakeUp();

			// 動く割合の算出
			float ratioA = 0, ratioB = 0;
			{
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
			}

			// ----- 位置の補正 ----- //
			{
				SimpleMath::Vector3 corrVecA = ratioA * hit.hitDir * hit.hitLen;
				SimpleMath::Vector3 corrVecB = ratioB * hit.hitDir * hit.hitLen;

				colA->GetTransform()->AddCache(corrVecA);
				colB->GetTransform()->AddCache(corrVecB);
			}

			// ----- 速度の補正 ----- //

			// --- 衝突法線方向 --- // 

			// 双方の速度を取得
			SimpleMath::Vector3 aVel = (aRigid ? aRigid->GetVelocity() : SimpleMath::Vector3::Zero);
			SimpleMath::Vector3 bVel = (bRigid ? bRigid->GetVelocity() : SimpleMath::Vector3::Zero);

			// 相対速度の計算
			SimpleMath::Vector3 relativeVel = bVel - aVel;

			// 投影して衝突法線方向の速度成分を計算
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
				float mu = (aFric + bFric) / 2;	// 平均値を扱う

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
		if (hasTrigger)
		{
			colA->GetOwn()->BaseOnTrigger(colB);
			colB->GetOwn()->BaseOnTrigger(colA);
		}
		else
		{
			colA->GetOwn()->BaseOnCollision(colB);
			colB->GetOwn()->BaseOnCollision(colA);
		}
	}
}
