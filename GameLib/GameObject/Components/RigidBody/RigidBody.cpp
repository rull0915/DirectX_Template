#include "pch.h"
#include "RigidBody.h"

#include "../Managers/PhysicsManager.h"
#include "../../GameObject.h"

using namespace DirectX;

RigidBody::RigidBody(GameObject* own)
    : BaseComponent(own, MAIN_RIGIDBODY, true)
    , m_pTransform{ own->GetComponent<Transform>() }
    , m_mass{ 1.0f }
    , m_invMass{ 1.0f / m_mass }
    , m_velocity{ 0.0f }
    , m_acceleration{ 0.0f }
    , m_force{ 0.0f }
    , m_friction{ 0.0f }
    , m_restitution{ 0.5f }
    , m_isStatic{ false }
    , m_useGravity{ true }
{
    // マネージャーへ自信を登録
    PhysicsManager::Instance().AddRigidBody(this);
}

void RigidBody::Integrate(float elapsedTime)
{
    // 加速度を力から計算
    SimpleMath::Vector3 acceleration = m_force * m_invMass;

    // 速度に反映
    m_velocity += acceleration * elapsedTime;

    // 座標に反映
    m_pTransform->AddWorldPosition(m_velocity * elapsedTime);

    // 貯まった力をリセット
    m_force = SimpleMath::Vector3::Zero;
}
