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

#include "GameLib/GameObject/Managers/2DManagers/PhysicsManager2D.h"
#include "GameLib/GameObject/GameObject.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

RigidBody2D::RigidBody2D(GameObject* own)
    : IRigidBody(own, MAIN_RIGIDBODY_2D)
    , m_velocity{ 0.0f }
    , m_acceleration{ 0.0f }
    , m_force{ 0.0f }
{
    // マネージャーへ自信を登録
    PhysicsManager2D::Instance().AddRigidBody2D(this);
}

void RigidBody2D::Integrate(float elapsedTime)
{
    AddStoppingTime(elapsedTime);

    // 速度を調べる
    if (m_velocity.LengthSquared() >= 0.1f) SetStoppintTime(0);

    if (GetStoppingTime() >= SLEEP_BORDER)
    {
        SetSleep(true);
        m_velocity = SimpleMath::Vector2::Zero;
        m_force = SimpleMath::Vector2::Zero;
    }

    if (IsSleep()) return;

    // 加速度を力から計算
    if (m_force == SimpleMath::Vector2::Zero) return;

    SimpleMath::Vector2 acceleration = m_force * GetInvMass();

    // 速度に反映
    m_velocity += acceleration * elapsedTime;

    // 座標に反映
    DirectX::SimpleMath::Vector3 vel3D = { m_velocity.x, m_velocity.y, 0 };
    GetTransform()->AddWorldPosition(vel3D * elapsedTime);

    // 貯まった力をリセット
    m_force = SimpleMath::Vector2::Zero;
}
