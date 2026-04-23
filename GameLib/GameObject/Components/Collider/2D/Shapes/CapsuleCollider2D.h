//====================================================//
// ファイル名   : CapsuleCollider2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : カプセルのコライダー
//
// 更新履歴 :
// 2026/03/18 新規作成
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
enum class AxisType2D
{
    Horizonatl,
    Vertical
};

class CapsuleCollider2D : public BaseCollider2D
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = SUB_COLLIDER_2D_CAPSULE;
    static constexpr bool IS_MAIN = false;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 使用する軸
    AxisType2D m_lineDir; // 正規化して使用

    // ラインの長さ
    float m_capsuleHeight;

    // 半径
    float m_radius;

    // 計算済みワールド情報
    struct WorldCache {
        float radius;
        float height;
        float lineLength;
        DirectX::SimpleMath::Vector2 dir;
        std::pair<DirectX::SimpleMath::Vector2, DirectX::SimpleMath::Vector2> points;
    };

    mutable WorldCache m_cache;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    CapsuleCollider2D(GameObject* own, float height = 1.0f, float radius = 0.5f, AxisType2D type = AxisType2D::Vertical, DirectX::SimpleMath::Vector2 pos = { 0, 0 })
        : BaseCollider2D(own, ColliderType2D::Capsule, SUB_COLLIDER_CAPSULE, false, pos)
        , m_lineDir{ type }
        , m_capsuleHeight{ height }
        , m_radius{ radius }
        , m_cache{}
    {
    };
    ~CapsuleCollider2D() = default;

    void UpdateCache() const override;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    // ラインの方向ベクトルを返す関数
    DirectX::SimpleMath::Vector2 GetLineDir() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.dir;
    }

    // 半径を返す関数
    float GetRadius() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.radius;
    }

    // カプセルの高さを返す関数
    float GetHeight() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.height;
    }

    // 線の長さを返す関数
    float GetLineLength() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.lineLength;
    }
    
    // Lineを構成する2点を返す関数
    std::pair<DirectX::SimpleMath::Vector2, DirectX::SimpleMath::Vector2> GetPoints() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.points;
    }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetHeight(float height) 
    {
        m_capsuleHeight = height;
        SetDirty();
    }

    void SetRadius(float radius) 
    {
        m_radius = radius;
        SetDirty();
    }

    // 描画関数
    void DebugDraw(int color) const override;
};