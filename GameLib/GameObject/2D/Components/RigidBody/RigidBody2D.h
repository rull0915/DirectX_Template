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
#include "../BaseComponent2D.h"

//====================================================//
// 前方宣言
//====================================================//
class GameObject2D;
class Transform2D;

//====================================================//
// クラス宣言
//====================================================//
class RigidBody2D : public BaseComponent2D
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
    
    // トランスフォームポインタ
    Transform2D* m_pTransform;

    // 質量
    float m_mass, m_invMass;    // 質量 / 質量の逆数

    // 速度/加速度
    DirectX::SimpleMath::Vector2 m_velocity, m_acceleration;

    // かかっている力
    DirectX::SimpleMath::Vector2 m_force;

    // 静止している時間
    float m_stoppingTime;

    // 係数
    float m_friction;       // 摩擦
    float m_restitution;    // 反発

    // フラグ
    bool m_isStatic;    // 固定
    bool m_useGravity;  // 重力の影響

public:
    bool m_isSleep;     // スリープフラグ

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    RigidBody2D(GameObject2D* own);

    ~RigidBody2D() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void Integrate(float elapsedTime);

    void AddForce(DirectX::SimpleMath::Vector2 vec)
    {
        if (m_isSleep) return;
        m_force += vec;
    }

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    float GetMass() const { return m_mass; }
    float GetInvMass() const { return m_invMass; }
    bool IsStatic() const { return m_isStatic; }
    bool IsUseGravity() const { return m_useGravity; }
    DirectX::SimpleMath::Vector2 GetForce() const { return m_force; }
    DirectX::SimpleMath::Vector2 GetVelocity() const { return m_velocity; }
    DirectX::SimpleMath::Vector2 GetAcceleration() const { return m_acceleration; }
    float GetFriction() const { return m_friction; }
    float GetRestitution() const { return m_restitution; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetMass(float mass)
    {
        m_mass = mass;
        m_invMass = 1.0f / mass;
    }
    void SetStatic(bool frag) { m_isStatic = frag; }
    void SetUseGravity(bool frag) { m_useGravity = frag; }
    void SetForce(DirectX::SimpleMath::Vector2 f) { m_force = f; }
    void SetVelocity(DirectX::SimpleMath::Vector2 v) { m_velocity = v; }
    void SetAcceleration(DirectX::SimpleMath::Vector2 a) { m_acceleration = a; }
    void SetFriction(float f) { m_friction = f; }
    void SetRestitution(float r) { m_restitution = r; }

    void WakeUp() 
    {
        m_isSleep = false; 
        m_velocity = DirectX::SimpleMath::Vector2::Zero;
    }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};