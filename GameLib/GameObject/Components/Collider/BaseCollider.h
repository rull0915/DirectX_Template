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
#include "../BaseComponent.h"
#include "../Transform/Transform.h"

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

    void DebugDraw(int color) const;
};

//====================================================//
// クラス宣言
//====================================================//
class BaseCollider : public BaseComponent
{
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

    // 自身のレイヤー
    int m_layerNum;

    // コライダーのタイプ
    ColliderType m_type;

protected:

    // 親のトランスフォーム
    const Transform* m_pTransform;

    // ローカル中心座標
    DirectX::SimpleMath::Vector3 m_localCenterPos;

    // 自身を覆うAABB
    mutable AABB m_boundingBox;

    // トリガーフラグ
    bool m_isTrigger;

    // 変更済みフラグ
    mutable bool m_isChanged;

    // 値が変更されているかのフラグ
    mutable bool m_isDirty;

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

    int GetLayer() const { return m_layerNum; }

    // 自身を覆うAABBを取得する関数
    inline AABB& GetBoundingBox() const
    {
        if(m_isDirty) UpdateCache();
        return m_boundingBox;
    }

    // ワールド座標系での中心座標を返す関数
    DirectX::SimpleMath::Vector3 GetWorldCenterPos() const
    {
        if(m_isDirty) UpdateCache();
        return m_worldCenterPos;
    }

    // トリガーフラグ
    bool IsTrigger() const { return m_isTrigger; }

    bool IsChanged() const { return m_isChanged; }
    bool IsDirty() const { return m_isDirty; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLayer(int num) 
    { 
        m_layerNum = std::min(num, 99); 
    }

    void SetDirty() { m_isDirty = true; };

    void SetLocalPos(DirectX::SimpleMath::Vector3 pos)
    {
        m_localCenterPos = pos;
        SetDirty();
    }

    void SetTrigger(bool frag) { m_isTrigger = frag; }

    void ResetChangeFrag() { m_isChanged = false; }

    //-----------------------------------------------------
    // その他関数
    //-----------------------------------------------------

    // デバッグ描画関数
    virtual void DebugDraw(int color) const = 0;
    void DebugDrawAABB(int color) const { m_boundingBox.DebugDraw(color); }

    // キャッシュの更新をする関数
    virtual void UpdateCache() const = 0;
};