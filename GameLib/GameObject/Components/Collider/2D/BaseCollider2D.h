//====================================================//
// ファイル名   : BaseCollider2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要 : 2Dコライダー基底クラス
//
// 更新履歴 :
// 2026/04/16 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../ICollider.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
//====================================================//
// 列挙型宣言
//====================================================//
enum class ColliderType2D
{
    Circle,
    Box,
    Capsule,
};

//====================================================//
// 構造体宣言
//====================================================//
struct AABB2D
{
    DirectX::SimpleMath::Vector2 min;
    DirectX::SimpleMath::Vector2 max;

    AABB2D(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b)
        : min{ a }
        , max{ b }
    {
    };

    AABB2D()
        : min{ 0, 0 }, max{ 0, 0 }
    {
    }

    void DebugDraw(Renderer& renderer, int color) const;
};

//====================================================//
// クラス宣言
//====================================================//
class BaseCollider2D : public ICollider
{
    // ----------------------------------------------------
    // friend指定
    // ----------------------------------------------------
    friend class CollideManager2D;

    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = MAIN_COLLIDER_2D;
    static constexpr bool IS_MAIN = true;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // コライダーのタイプ
    ColliderType2D m_type;

    // ローカル中心座標
    DirectX::SimpleMath::Vector2 m_localCenterPos;

    // 自身を覆うAABB2D
    mutable AABB2D m_boundingBox;

    // ワールド中心座標のキャッシュ
    mutable DirectX::SimpleMath::Vector2 m_worldCenterPos;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BaseCollider2D(GameObject* own, ColliderType2D type, int ID, bool isMain, DirectX::SimpleMath::Vector2 center = { 0, 0 });

    virtual ~BaseCollider2D() = default;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    ColliderType2D GetType() const { return m_type; };

    inline AABB2D& GetBoundingBox() const { return m_boundingBox; } // 自身を覆うAABBを取得する関数

    // ワールド座標系での中心座標を返す関数
    DirectX::SimpleMath::Vector2 GetWorldCenterPos() const
    {
        if (IsDirty()) UpdateCache();
        return m_worldCenterPos;
    }
    // ローカル座標系
    DirectX::SimpleMath::Vector2 GetLocalCenterPos() const { return m_localCenterPos; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLocalPos(DirectX::SimpleMath::Vector2 pos)
    {
        m_localCenterPos = pos;
        SetDirty();
    }

    //-----------------------------------------------------
    // その他関数
    //-----------------------------------------------------

    // デバッグ描画関数
    virtual void DebugDraw(Renderer& renderer, int color) const override = 0;
    void DebugDrawAABB(Renderer& renderer, int color) const override { m_boundingBox.DebugDraw(renderer, color); }

    // キャッシュの更新をする関数
    virtual void UpdateCache() const = 0;

protected:

    inline void SetWorldPosition(const DirectX::SimpleMath::Vector2& pos) const { m_worldCenterPos = pos; }
    inline void SetBoundingBox(const AABB2D& box) const { m_boundingBox = box; }
};