//====================================================//
// ファイル名   : SphereCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : 球状のコライダー
//
// 更新履歴 :
// 2026/03/18 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseCollider.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class SphereCollider : public BaseCollider
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = SUB_COLLIDER_SPHERE;
    static constexpr bool IS_MAIN = false;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 半径
    float m_radius;

    mutable float m_worldRadius;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    SphereCollider(GameObject* own, float radius = 0.5f, DirectX::SimpleMath::Vector3 pos = {0, 0, 0})
        : BaseCollider(own, ColliderType::Sphere, SUB_COLLIDER_SPHERE, false, pos)
        , m_radius{ radius }
        , m_worldRadius{ 1 }
    {
    };
    ~SphereCollider() = default;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    float GetRadius() const 
    {
        UpdateCache();
        return m_worldRadius;
    }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetRadius(float rad)
    {
        m_radius = rad;
        SetDirty();
    }

    void UpdateCache() const override;
    void DebugDraw(int color) const override;
};