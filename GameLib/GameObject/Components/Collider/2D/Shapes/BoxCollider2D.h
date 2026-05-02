//====================================================//
// ファイル名   : BoxCollider2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要 : 2Dボックスコライダー
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
class BoxCollider2D : public BaseCollider2D
{
     // ----------------------------------------------------
     // 定数宣言
     // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = SUB_COLLIDER_2D_BOX;
    static constexpr bool IS_MAIN = false;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // ボックスのローカルでのサイズ
    DirectX::SimpleMath::Vector2 m_localSize;

    // 計算済みワールド情報
    struct WorldCache {
        float angle;

        DirectX::SimpleMath::Vector2 xAxis;
        DirectX::SimpleMath::Vector2 yAxis;

        DirectX::SimpleMath::Vector2 scale;
    };

    mutable WorldCache m_cache;


public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BoxCollider2D(GameObject* own, DirectX::SimpleMath::Vector2 size = { 1, 1 }, DirectX::SimpleMath::Vector2 pos = { 0, 0 })
        : BaseCollider2D(own, ColliderType2D::Box, SUB_COLLIDER_2D_BOX, false, pos)
        , m_localSize{ size }
        , m_cache{}
    {
    };
    ~BoxCollider2D() = default;

    void UpdateCache() const override;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    // 各軸ベクトルを取得する関数
    float GetAngle() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.angle;
    }
    DirectX::SimpleMath::Vector2 GetXAxis() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.xAxis;
    }
    DirectX::SimpleMath::Vector2 GetYAxis() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.yAxis;
    }
    DirectX::SimpleMath::Vector2 GetSize() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.scale;
    }
    DirectX::SimpleMath::Vector2 GetHalfSize() const
    {
        if (IsDirty()) UpdateCache();
        return m_cache.scale * 0.5f;
    }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetLocalSize(DirectX::SimpleMath::Vector2 scale)
    {
        m_localSize = scale;
        SetDirty();
    }

    // 描画関数
    void DebugDraw(Renderer& renderer, int color) const override;
};