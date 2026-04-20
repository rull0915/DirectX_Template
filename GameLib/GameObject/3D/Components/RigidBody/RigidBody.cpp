//====================================================//
// ファイル名  : RigidBody.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/06
//
// 概要        :  RigidBodyコンポーネント
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "RigidBody.h"

#include "GameLib/GameObject/3D/Managers/PhysicsManager.h"
#include "GameLib/GameObject/3D/GameObject.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

RigidBody::RigidBody(GameObject* own)
    : BaseComponent(own, MAIN_RIGIDBODY, true)
    , m_pTransform{ own->GetComponent<Transform>() }
    , m_mass{ 1.0f }
    , m_invMass{ 1.0f }
    , m_velocity{ 0.0f }
    , m_acceleration{ 0.0f }
    , m_force{ 0.0f }
    , m_friction{ 0.0f }
    , m_restitution{ 0.5f }
    , m_isStatic{ false }
    , m_useGravity{ true }
    , m_isSleep{ false }
    , m_stoppingTime{ 0 }
{
    // マネージャーへ自信を登録
    PhysicsManager::Instance().AddRigidBody(this);
}

void RigidBody::Integrate(float elapsedTime)
{
    m_stoppingTime += elapsedTime;

    // 速度を調べる
    if (m_velocity.LengthSquared() >= 0.1f) m_stoppingTime = 0;

    if (m_stoppingTime >= SLEEP_BORDER)
    {
        m_isSleep = true;
        m_velocity = SimpleMath::Vector3::Zero;
        m_force = SimpleMath::Vector3::Zero;
    }

    if (m_isSleep) return;

    // 加速度を力から計算
    if (m_force == SimpleMath::Vector3::Zero) return;

    SimpleMath::Vector3 acceleration = m_force * m_invMass;

    // 速度に反映
    m_velocity += acceleration * elapsedTime;

    // 座標に反映
    m_pTransform->AddWorldPosition(m_velocity * elapsedTime);

    // 貯まった力をリセット
    m_force = SimpleMath::Vector3::Zero;
}
