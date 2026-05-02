//====================================================//
// ファイル名   : BaseCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : 衝突判定の基底クラス
//
// 更新履歴 :
// 2026/03/18 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../ICollider.h"

//====================================================//
// 列挙型宣言
//====================================================//
enum class ColliderType
{
    Sphere,
    Line,
    Capsule,
    Box,
};

//====================================================//
// 構造体宣言
//====================================================//
struct AABB
{
    DirectX::SimpleMath::Vector3 min;
    DirectX::SimpleMath::Vector3 max;

    AABB(DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b)
        : min{ a }
        , max{ b }
    {
    };

    AABB() 
        : min{ 0, 0, 0 }, max{ 0, 0, 0 }
    {}

    void DebugDraw(Renderer& renderer, int color) const;
};

//====================================================//
// クラス宣言
//====================================================//
class BaseCollider : public ICollider
{
    // ----------------------------------------------------
    // friend指定
    // ----------------------------------------------------
    friend class CollideManager;

    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = MAIN_COLLIDER;
    static constexpr bool IS_MAIN = true;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------
    
    // コライダーのタイプ
    ColliderType m_type;

    // ローカル中心座標
    DirectX::SimpleMath::Vector3 m_localCenterPos;

    // 自身を覆うAABB
    mutable AABB m_boundingBox;

    // ワールド中心座標のキャッシュ
    mutable DirectX::SimpleMath::Vector3 m_worldCenterPos;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BaseCollider(GameObject* own, ColliderType type, int ID, bool isMain, DirectX::SimpleMath::Vector3 center = { 0, 0, 0 });

    virtual ~BaseCollider();

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    ColliderType GetType() const { return m_type; };

    inline AABB& GetBoundingBox() const{ return m_boundingBox; } // 自身を覆うAABBを取得する関数

    // ワールド座標系での中心座標を返す関数
    DirectX::SimpleMath::Vector3 GetWorldCenterPos() const
    {
        if (IsDirty()) UpdateCache();
        return m_worldCenterPos;
    }
    // ローカル座標系
    DirectX::SimpleMath::Vector3 GetLocalCenterPos() const { return m_localCenterPos; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLocalPos(DirectX::SimpleMath::Vector3 pos)
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

    inline void SetWorldPosition(const DirectX::SimpleMath::Vector3& pos) const { m_worldCenterPos = pos; }
    inline void SetBoundingBox(const AABB& box) const { m_boundingBox = box; }
};