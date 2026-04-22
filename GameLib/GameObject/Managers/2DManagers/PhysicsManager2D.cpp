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
	for (auto* body : m_rigidBodies) 
	{
		if (body->IsStatic() || !body->IsUseGravity()) continue;
		body->AddForce(m_gravityVec * m_gravityPower * body->GetMass());
	}

	// 位置の更新
	for (auto* body : m_rigidBodies) 
	{
		if (body->IsStatic()) continue;

		body->Integrate(elapsedTime);
	}
}
