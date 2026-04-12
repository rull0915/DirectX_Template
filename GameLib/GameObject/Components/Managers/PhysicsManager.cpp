#include "pch.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager()
	: m_removeReserves{}
	, m_reserves{}
	, m_rigidBodies{}
{

}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Update(float elapsedTime)
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
