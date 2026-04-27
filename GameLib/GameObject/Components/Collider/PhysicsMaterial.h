//====================================================//
// ファイル名   : PhysicsMaterial.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/27
//
// 概要 : 摩擦、反発の設定
//
// 更新履歴 :
// 2026/04/27 新規作成
//====================================================//

#pragma once
#include "GameLib/GameMath/GameMath.h"

//====================================================//
// 構造体宣言
//====================================================//

enum class CombineMode
{
    Average ,
    Minimum ,
    Multiply,
    Maximum ,
};

class PhysicsMaterial
{
private:
    float m_staticFriction;
    float m_dynamicFriction;
    float m_bounciness;

    CombineMode m_frictionCombine;
    CombineMode m_bounceCombine;

public:
    constexpr PhysicsMaterial()
        : m_staticFriction{ 0.6f }
        , m_dynamicFriction{ 0.6f }
        , m_bounciness{ 0 }
        , m_frictionCombine{ CombineMode::Average }
        , m_bounceCombine{ CombineMode::Average }
    {
    };
    constexpr PhysicsMaterial(float sFric, float dFric, float bounce, CombineMode fMode, CombineMode bMode)
        : m_staticFriction{ sFric }
        , m_dynamicFriction{ dFric }
        , m_bounciness{ bounce }
        , m_frictionCombine{ fMode }
        , m_bounceCombine{ bMode }
    {
    };

    //--------------------------------------------
    // ゲッター
    //--------------------------------------------
    constexpr inline float GetStaticFriction() const { return m_staticFriction; }
    constexpr inline float GetDynamicFriction() const { return m_dynamicFriction; }
    constexpr inline float GetBounciness() const { return m_bounciness; }

    constexpr inline CombineMode GetFrictionCombine() const { return m_frictionCombine; }
    constexpr inline CombineMode GetBounceCombine() const { return m_bounceCombine; }

    //--------------------------------------------
    // セッター
    //--------------------------------------------
    inline void SetStaticFriction(float value) { m_staticFriction = std::max(value, 0.0f); }
    inline void SetDynamicFriction(float value) { m_dynamicFriction = std::max(value, 0.0f); }
    inline void SetBounciness(float value) { m_bounciness = MyMath::Clamp(value, 0.0f, 1.0f); }

    inline void SetFrictionCombine(CombineMode mode) { m_frictionCombine = mode; }
    inline void SetBounceCombine(CombineMode mode) { m_bounceCombine = mode; }
};

// ===== その他 ====== //

namespace Physics
{
    // デフォルトの値
    static constexpr PhysicsMaterial DEFAULT_MATERIAL = PhysicsMaterial(0.6f, 0.6f, 0.6f, CombineMode::Average, CombineMode::Average);

    /// <summary>
    /// 優先度を取得する関数
    /// </summary>
    /// <param name="mode">モード</param>
    /// <returns>値</returns>
    constexpr int GetPriority(CombineMode mode)
    {
        switch (mode)
        {
        case CombineMode::Average:  return 0;
        case CombineMode::Minimum:  return 1;
        case CombineMode::Multiply: return 2;
        case CombineMode::Maximum:  return 3;
        }
        return -1;
    }

    /// <summary>
    /// 使用する値を取得する関数
    /// </summary>
    /// <param name="valueA">1つ目の値</param>
    /// <param name="valueB">2つ目の値</param>
    /// <param name="modeA">1つ目のモード</param>
    /// <param name="modeB">2つ目のモード</param>
    /// <returns></returns>
    constexpr inline float GetValue(float valueA, float valueB, CombineMode modeA, CombineMode modeB)
    {
        // 優先度の高いモードを使用
        CombineMode mode = GetPriority(modeA) > GetPriority(modeB) ? modeA : modeB;

        switch (mode)
        {
            // 平均値
        case CombineMode::Average:
            return (valueA + valueB) / 2;
            // 最大値
        case CombineMode::Maximum:
            return std::max(valueA, valueB);
            // 最小値
        case CombineMode::Minimum:
            return std::min(valueA, valueB);
            // 乗算
        case CombineMode::Multiply:
            return valueA * valueB;
        default:
            return -1;
        }
    }
}
