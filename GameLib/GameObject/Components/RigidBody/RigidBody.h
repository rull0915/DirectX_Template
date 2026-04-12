//====================================================//
// ファイル名   : RigidBody.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/06
//
// 概要 : RigidBodyクラス 質量や速度などを管理します
//
// 更新履歴 :
// 2026/04/06 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseComponent.h"

//====================================================//
// 前方宣言
//====================================================//
class GameObject;
class Transform;

//====================================================//
// クラス宣言
//====================================================//
class RigidBody : public BaseComponent
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = MAIN_RIGIDBODY;
    static constexpr bool IS_MAIN = true;
    static constexpr float SLEEP_BORDER = 2.0f;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------
    
    // トランスフォームポインタ
    Transform* m_pTransform;

    // 質量
    float m_mass, m_invMass;    // 質量 / 質量の逆数

    // 速度/加速度
    DirectX::SimpleMath::Vector3 m_velocity, m_acceleration;

    // かかっている力
    DirectX::SimpleMath::Vector3 m_force;

    // 前フレームの位置
    DirectX::SimpleMath::Vector3 m_oldPosition;
    DirectX::SimpleMath::Vector3 m_nowPosition;

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
    RigidBody(GameObject* own);

    ~RigidBody() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void Integrate(float elapsedTime);

    void AddForce(DirectX::SimpleMath::Vector3 vec)
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
    DirectX::SimpleMath::Vector3 GetForce() const { return m_force; }
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
    DirectX::SimpleMath::Vector3 GetAcceleration() const { return m_acceleration; }
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
    void SetForce(DirectX::SimpleMath::Vector3 f) { m_force = f; }
    void SetVelocity(DirectX::SimpleMath::Vector3 v) { m_velocity = v; }
    void SetAcceleration(DirectX::SimpleMath::Vector3 a) { m_acceleration = a; }
    void SetFriction(float f) { m_friction = f; }
    void SetRestitution(float r) { m_restitution = r; }

    void WakeUp() 
    {
        m_isSleep = false; 
    }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};