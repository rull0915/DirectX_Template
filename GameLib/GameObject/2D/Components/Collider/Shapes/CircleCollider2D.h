//====================================================//
// ファイル名   : CircleCollider2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要 : 2Dの円衝突クラス
//
// 更新履歴 :
// 2026/04/17 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseCollider2D.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class CircleCollider2D : public BaseCollider2D
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = SUB_COLLIDER_2D_CIRCLE;
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
    CircleCollider2D(GameObject2D* own, float radius = 0.5f, DirectX::SimpleMath::Vector2 pos = { 0, 0 })
        : BaseCollider2D(own, ColliderType2D::Circle, SUB_COLLIDER_2D_CIRCLE, false, pos)
        , m_radius{ radius }
        , m_worldRadius{ 1 }
    {
    };
    ~CircleCollider2D() = default;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    float GetRadius() const
    {
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