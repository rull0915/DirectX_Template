//====================================================//
// ファイル名  : PhysicsManager2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要        :  2D版物理挙動管理クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "PhysicsManager2D.h"
#include "Collider/CollideManager2D.h"

#include "GameLib/GameMath/GameMath.h"

//====================================================//
// 関数の実体宣言
//====================================================//

PhysicsManager2D::PhysicsManager2D()
	: m_removeReserves{}
	, m_reserves{}
	, m_rigidBodies{}
{

}

PhysicsManager2D::~PhysicsManager2D()
{
}

void PhysicsManager2D::Update(float elapsedTime)
{
	// 予約されているRigidBodyの追加
	AddReserved();

	// 全RigidBodyの更新

	// 外力の適用

	// 重力
	for (auto* body : m_rigidBodies)
	{
		if (body->IsStatic() || !body->IsUseGravity()) continue;

		if(!body->IsSleep()) body->AddForce(m_gravityVec * m_gravityPower * body->GetMass(), false);
	}

	// 位置の更新
	for (auto* body : m_rigidBodies)
	{
		if (body->IsStatic()) continue;

		body->Integrate(elapsedTime);
	}

	// --- 衝突判定 --- //

	// 空間分割の更新
	CollideManager2D::Instance().MoveAllColliderOnTree();

	// 判定
	CollideManager2D::Instance().CheckHitAll(m_contacts);

	// 位置補正
	HittedCorrection();
}

// 衝突後の補正を行う関数
void PhysicsManager2D::HittedCorrection()
{
	// ペアリストの初期化
	m_contactMap.clear();

	for (auto& contact : m_contacts)
	{
		// レイヤー管理

		// リストに追加
		ObjectPair pair = ObjectPair(contact.a, contact.b);
		m_contactMap.insert(std::make_pair(pair, contact));

		// どちらも起こす
		if (!contact.bIsStatic && (contact.aRigid && contact.aRigid->IsSleep())) contact.aRigid->WakeUp();
		if (!contact.aIsStatic && (contact.bRigid && contact.bRigid->IsSleep())) contact.bRigid->WakeUp();

		// トリガーチェック
		if (contact.isTrigger) { continue; }

		// 位置補正
		PositionCorrection(contact);

		// 速度補正
		VelocityCorrection(contact);
	}
}

void PhysicsManager2D::PositionCorrection(HitContact2D& contact)
{
	auto aRigid = contact.aRigid, bRigid = contact.bRigid;

	// 動く割合の算出
	float ratioA = 0, ratioB = 0;
	{
		// どちらかがStaticな場合
		if (contact.aIsStatic) {
			ratioA = 0;   // Aは動かない
			ratioB = 1;   // Bが100%押し戻される
		}
		else if (contact.bIsStatic) {
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
		DirectX::SimpleMath::Vector2 corrVecA = ratioA * contact.normal * contact.penetration;
		DirectX::SimpleMath::Vector2 corrVecB = ratioB * contact.normal * contact.penetration;

		contact.a->GetComponent<Transform>()->AddCache({ corrVecA });
		contact.b->GetComponent<Transform>()->AddCache({ corrVecB });
	}
}

void PhysicsManager2D::VelocityCorrection(HitContact2D& contact)
{
	// ----- 速度の補正 ----- //
	auto aRigid = contact.aRigid, bRigid = contact.bRigid;

	// --- 衝突法線方向 --- // 

	// 双方の速度を取得
	DirectX::SimpleMath::Vector2 aVel = (aRigid ? aRigid->GetVelocity() : DirectX::SimpleMath::Vector2::Zero);
	DirectX::SimpleMath::Vector2 bVel = (bRigid ? bRigid->GetVelocity() : DirectX::SimpleMath::Vector2::Zero);

	// 相対速度の計算
	DirectX::SimpleMath::Vector2 relativeVel = bVel - aVel;

	// 投影して衝突法線方向の速度成分を計算
	float hitDirVel = relativeVel.Dot(contact.normal);

	// ぶつかっていれば
	if (hitDirVel < 0)
	{
		// 双方の物理マテリアルを取得
		PhysicsMaterial aMat = contact.aCol->GetPhysicsMaterial(), bMat = contact.bCol->GetPhysicsMaterial();

		// 反発係数の小さいほうを適用
		float aBounce = (!contact.aIsStatic ? aMat.GetBounciness() : 1), bBounce = (!contact.bIsStatic ? bMat.GetBounciness() : 1);
		float e = Physics::GetValue(aBounce, bBounce, aMat.GetBounceCombine(), bMat.GetBounceCombine());

		// インパルスの算出

		// それぞれの質量の逆数を取得
		float aInvMass = aRigid ? aRigid->GetInvMass() : 0, bInvMass = bRigid ? bRigid->GetInvMass() : 0;
		float j = (-(1 + e) * hitDirVel) / (aInvMass + bInvMass);	// 打ち消しと反発を同時に行うために-(1 + e)

		// 法線方向のインパルス
		DirectX::SimpleMath::Vector2 normalImpulse = j * contact.normal;

		// --- 接線方向の処理 --- //

		// 摩擦係数を取得
		float s_aFric = aMat.GetStaticFriction(), s_bFric = bMat.GetStaticFriction();
		float d_aFric = aMat.GetDynamicFriction(), d_bFric = bMat.GetDynamicFriction();

		float s_mu = Physics::GetValue(s_aFric, s_bFric, aMat.GetFrictionCombine(), bMat.GetFrictionCombine());
		float d_mu = Physics::GetValue(d_aFric, d_bFric, aMat.GetFrictionCombine(), bMat.GetFrictionCombine());

		// 接線ベクトルの作成
		// 相対速度から法線方向の成分を抜き出す
		DirectX::SimpleMath::Vector2 normalVel = hitDirVel * contact.normal;

		// 全体の相対速度から法線成分を引くことで接線方向の成分にする
		DirectX::SimpleMath::Vector2 tangentVel = relativeVel - normalVel;

		// 正規化して接線とする
		float tangentSpeed = tangentVel.Length();
		DirectX::SimpleMath::Vector2 tangential = DirectX::SimpleMath::Vector2::Zero;

		if (tangentSpeed > 0.0001f) {
			tangential = tangentVel / tangentSpeed;
		}

		// 接線方向の速度をゼロにするインパルスを計算
		float j_tangent = -(relativeVel.Dot(tangential)) / (aInvMass + bInvMass);

		// 静止摩擦力の限界値を計算
		float maxStaticFriction = s_mu * abs(j);

		float actualFrictionImpulse;

		if (abs(j_tangent) < maxStaticFriction)
		{
			// 静止摩擦
			actualFrictionImpulse = j_tangent;
		}
		else
		{
			// 動摩擦
			float maxDynamicFriction = d_mu * abs(j);
			actualFrictionImpulse = MyMath::Clamp(j_tangent, -maxDynamicFriction, maxDynamicFriction);
		}

		// 接線方向のインパルス
		DirectX::SimpleMath::Vector2 tangentImpulse = actualFrictionImpulse * tangential;


		// 双方の速度を変更
		if (aRigid) aRigid->AddImpulse(-(tangentImpulse + normalImpulse), false);
		if (bRigid) bRigid->AddImpulse(tangentImpulse + normalImpulse, false);
	}
}

