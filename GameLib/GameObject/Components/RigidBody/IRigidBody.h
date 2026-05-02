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
// 列挙型宣言
//====================================================//
enum class ForceMode
{
    Force,
    Acceleration,
    Impulse,
    VelocityChange,
};

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

    // 減衰率
    float m_linearDamping;  // 減衰率

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
    inline float GetLinearDamping() const { return m_linearDamping; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetMass(float mass)
    {
        m_mass = mass;
        m_invMass = 1.0f / mass;
    }
    void SetStatic(bool frag) { m_isStatic = frag; }
    void SetUseGravity(bool frag) 
    { 
        m_useGravity = frag;
        WakeUp();
    }
    void SetSleep(bool frag) { m_isSleep = frag; }
    void SetLinearDamping(float r) { m_linearDamping = std::max(0.0f, r); }

    //-----------------------------------------------------
    // その他関数
    //-----------------------------------------------------
    virtual void Integrate(float elapsedTime) = 0;

    void WakeUp()
    {
        SetSleep(false);
        SetStoppintTime(0);
    }

    // protected関数群
protected:
    Transform* GetTransform() const { return m_pTransform; }

    float GetStoppingTime() const { return m_stoppingTime; }
    void AddStoppingTime(float time) { m_stoppingTime += time; }
    void SetStoppintTime(float time) { m_stoppingTime = time; }
};