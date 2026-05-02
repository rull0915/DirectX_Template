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
#include "../IRigidBody.h"

//====================================================//
// 前方宣言
//====================================================//
class GameObject;
class Transform;

//====================================================//
// クラス宣言
//====================================================//
class RigidBody : public IRigidBody
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
    
    // 速度/加速度
    DirectX::SimpleMath::Vector3 m_velocity, m_acceleration;

    // かかっている力
    DirectX::SimpleMath::Vector3 m_force;

    // 前フレームの位置
    DirectX::SimpleMath::Vector3 m_oldPosition;
    DirectX::SimpleMath::Vector3 m_nowPosition;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    RigidBody(GameObject* own);

    ~RigidBody() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void Integrate(float elapsedTime) override;

    void AddForce(DirectX::SimpleMath::Vector3 vec, ForceMode mode = ForceMode::Force, bool wakeUp = true)
    {
        switch (mode)
        {
        case ForceMode::Force:
            m_force += vec;
            break;
        case ForceMode::Acceleration:
            m_force += vec *  GetMass();
            break;
        case ForceMode::Impulse:
            m_velocity += vec * GetInvMass();
            break;
        case ForceMode::VelocityChange:
            m_velocity += vec;
            break;
        default:
            break;
        }

        if (wakeUp) WakeUp();
    }

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    DirectX::SimpleMath::Vector3 GetForce() const { return m_force; }
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
    DirectX::SimpleMath::Vector3 GetAcceleration() const { return m_acceleration; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetForce(DirectX::SimpleMath::Vector3 f) { m_force = f; }
    void SetVelocity(DirectX::SimpleMath::Vector3 v) { m_velocity = v; }
    void SetAcceleration(DirectX::SimpleMath::Vector3 a) { m_acceleration = a; }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};