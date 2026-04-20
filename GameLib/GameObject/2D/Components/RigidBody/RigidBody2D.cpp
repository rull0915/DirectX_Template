//====================================================//
// ファイル名  : RigidBody2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/06
//
// 概要        :  RigidBody2Dコンポーネント
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "RigidBody2D.h"

#include "GameLib/GameObject/2D/Managers/PhysicsManager2D.h"
#include "GameLib/GameObject/2D/GameObject2D.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

RigidBody2D::RigidBody2D(GameObject2D* own)
    : BaseComponent2D(own, MAIN_RIGIDBODY_2D, true)
    , m_pTransform{ own->GetComponent<Transform2D>() }
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
    PhysicsManager2D::Instance().AddRigidBody2D(this);
}

void RigidBody2D::Integrate(float elapsedTime)
{
    m_stoppingTime += elapsedTime;

    // 速度を調べる
    if (m_velocity.LengthSquared() >= 0.1f) m_stoppingTime = 0;

    //if (m_stoppingTime >= SLEEP_BORDER)
    //{
    //    m_isSleep = true;
    //    m_velocity = SimpleMath::Vector2::Zero;
    //    m_force = SimpleMath::Vector2::Zero;
    //}

    if (m_isSleep) return;

    // 加速度を力から計算
    if (m_force == SimpleMath::Vector2::Zero) return;

    SimpleMath::Vector2 acceleration = m_force * m_invMass;

    // 速度に反映
    m_velocity += acceleration * elapsedTime;

    // 座標に反映
    m_pTransform->AddWorldPosition(m_velocity * elapsedTime);

    // 貯まった力をリセット
    m_force = SimpleMath::Vector2::Zero;
}
