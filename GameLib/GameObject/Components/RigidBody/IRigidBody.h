//====================================================//
// ファイル名   : IRigidBody.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/22
//
// 概要 : リジッドボディ基底クラス
//
// 更新履歴 :
// 2026/04/22 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseComponent.h"
#include "../Transform/Transform.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class IRigidBody : public BaseComponent
{
private:
    // トランスフォームポインタ
    Transform* m_pTransform;

    // 質量
    float m_mass, m_invMass;    // 質量 / 質量の逆数

    // 静止している時間
    float m_stoppingTime;

    // 係数
    float m_friction;       // 摩擦
    float m_restitution;    // 反発

    // フラグ
    bool m_isStatic;    // 固定
    bool m_useGravity;  // 重力の影響

    bool m_isSleep;     // スリープフラグ

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    IRigidBody(GameObject* own, int id);

    virtual ~IRigidBody() = default;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    inline float GetMass() const { return m_mass; }
    inline float GetInvMass() const { return m_invMass; }
    inline bool IsStatic() const { return m_isStatic; }
    inline bool IsUseGravity() const { return m_useGravity; }
    inline bool IsSleep() const { return m_isSleep; }
    inline float GetFriction() const { return m_friction; }
    inline float GetRestitution() const { return m_restitution; }

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
    void SetSleep(bool frag) { m_isSleep = frag; }
    void SetFriction(float f) { m_friction = f; }
    void SetRestitution(float r) { m_restitution = r; }

    //-----------------------------------------------------
    // その他関数
    //-----------------------------------------------------
    virtual void Integrate(float elapsedTime) = 0;

    virtual void WakeUp() = 0;

    // protected関数群
protected:
    Transform* GetTransform() const { return m_pTransform; }

    float GetStoppingTime() const { return m_stoppingTime; }
    void AddStoppingTime(float time) { m_stoppingTime += time; }
    void SetStoppintTime(float time) { m_stoppingTime = time; }
};