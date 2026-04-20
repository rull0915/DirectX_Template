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
#include "../BaseComponent2D.h"
#include "../Transform/Transform2D.h"

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
    Triangle,
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

    void DebugDraw(int color) const;
};

//====================================================//
// クラス宣言
//====================================================//
class BaseCollider2D : public BaseComponent2D
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

    // 自身のレイヤー
    int m_layerNum;

    // コライダーのタイプ
    ColliderType2D m_type;

protected:

    // 親のトランスフォーム
    const Transform2D* m_pTransform;

    // ローカル中心座標
    DirectX::SimpleMath::Vector2 m_localCenterPos;

    // 自身を覆うAABB2D
    mutable AABB2D m_boundingBox;

    // トリガーフラグ
    bool m_isTrigger;

    // 変更済みフラグ
    mutable bool m_isChanged;

    // 値が変更されているかのフラグ
    mutable bool m_isDirty;

    // ワールド中心座標のキャッシュ
    mutable DirectX::SimpleMath::Vector2 m_worldCenterPos;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BaseCollider2D(GameObject2D* own, ColliderType2D type, int ID, bool isMain, DirectX::SimpleMath::Vector2 center = { 0, 0 });

    virtual ~BaseCollider2D() = default;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    ColliderType2D GetType() const { return m_type; };

    int GetLayer() const { return m_layerNum; }

    // 自身を覆うAABB2Dを取得する関数
    inline AABB2D& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    // ワールド座標系での中心座標を返す関数
    DirectX::SimpleMath::Vector2 GetWorldCenterPos() const
    {
        return m_worldCenterPos;
    }

    // トリガーフラグ
    bool IsTrigger() const { return m_isTrigger; }

    inline bool IsChanged() const { return m_isChanged; }
    inline bool IsDirty() const { return m_isDirty; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLayer(int num)
    {
        m_layerNum = std::min(num, 99);
    }

    void SetDirty() { m_isDirty = true; };

    void SetLocalPos(DirectX::SimpleMath::Vector2 pos)
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