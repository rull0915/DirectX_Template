//====================================================//
// ファイル名   : RigidBody2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/06
//
// 概要 : RigidBody2Dクラス 質量や速度などを管理します
//
// 更新履歴 :
// 2026/04/06 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../IRigidBody.h"

//====================================================//
// クラス宣言
//====================================================//
class RigidBody2D : public IRigidBody
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = MAIN_RIGIDBODY_2D;
    static constexpr bool IS_MAIN = true;
    static constexpr float SLEEP_BORDER = 2.0f;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 速度/加速度
    DirectX::SimpleMath::Vector2 m_velocity, m_acceleration;

    // かかっている力
    DirectX::SimpleMath::Vector2 m_force;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    RigidBody2D(GameObject* own);

    ~RigidBody2D() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void Integrate(float elapsedTime);

    void AddForce(DirectX::SimpleMath::Vector2 vec, bool wakeUp = true)
    {
        m_force += vec;
        if(wakeUp) WakeUp();
    }
    void AddImpulse(DirectX::SimpleMath::Vector2 impulse)
    {
        m_velocity += impulse * GetInvMass();
        WakeUp();
    }

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    DirectX::SimpleMath::Vector2 GetForce() const { return m_force; }
    DirectX::SimpleMath::Vector2 GetVelocity() const { return m_velocity; }
    DirectX::SimpleMath::Vector2 GetAcceleration() const { return m_acceleration; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetForce(DirectX::SimpleMath::Vector2 f) { m_force = f; }
    void SetVelocity(DirectX::SimpleMath::Vector2 v) { m_velocity = v; }
    void SetAcceleration(DirectX::SimpleMath::Vector2 a) { m_acceleration = a; }

    void WakeUp() 
    {
        SetSleep(false);
        if(m_velocity.LengthSquared() > 0.1f) SetStoppintTime(0);
    }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};