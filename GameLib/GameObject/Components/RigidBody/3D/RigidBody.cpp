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

#include "GameLib/GameObject/Managers/3DManagers/PhysicsManager.h"
#include "GameLib/GameObject/GameObject.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

RigidBody::RigidBody(GameObject* own)
    : IRigidBody(own, MAIN_RIGIDBODY_2D)
    , m_velocity{ 0.0f }
    , m_acceleration{ 0.0f }
    , m_force{ 0.0f }
{
    // マネージャーへ自信を登録
    PhysicsManager::Instance().AddRigidBody(this);
}

void RigidBody::Integrate(float elapsedTime)
{
    AddStoppingTime(elapsedTime);

    // 速度を調べる
    if (m_velocity.LengthSquared() >= 0.1f) SetStoppintTime(0);

    if (GetStoppingTime() >= SLEEP_BORDER)
    {
        SetSleep(true);
        m_velocity = SimpleMath::Vector3::Zero;
        m_force = SimpleMath::Vector3::Zero;
    }

    if (IsSleep()) return;

    // 加速度を力から計算
    if (m_force == SimpleMath::Vector3::Zero) return;

    SimpleMath::Vector3 acceleration = m_force * GetInvMass();

    // 速度に反映
    m_velocity += acceleration * elapsedTime;

    // 座標に反映
    GetTransform()->AddWorldPosition(m_velocity * elapsedTime);

    // 貯まった力をリセット
    m_force = SimpleMath::Vector3::Zero;
}
