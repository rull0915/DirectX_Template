//====================================================//
// ファイル名   : Transform2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要 :
//
// 更新履歴 :
// 2026/04/16 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseComponent2D.h"
#include <unordered_set>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//

class Transform2D : public BaseComponent2D
{
    // キャッシュ用構造体
    struct Cache
    {
        DirectX::SimpleMath::Vector2 position;
        float rotation;
        DirectX::SimpleMath::Vector2 scale;
    };

public:
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
    static constexpr int TYPE_ID = MAIN_TRANSFORM_2D;
    static constexpr bool IS_MAIN = true;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 情報
    DirectX::SimpleMath::Vector2 m_localPosition;
    float m_localRotation;
    DirectX::SimpleMath::Vector2 m_localScale;

    mutable bool m_isDirty; // 値が変わったらtrueにするフラグ

    mutable Cache m_cache;

    // --- 親子関係 --- //

    Transform2D* m_pParent;   // 親

    std::unordered_set<Transform2D*> m_children; // 子

    // 位置の変更をためておく変数
    DirectX::SimpleMath::Vector2 m_addCache;
    bool m_changeCache;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    Transform2D(GameObject2D* own);
    ~Transform2D();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // 親を設定する関数
    void SetParent(Transform2D* parent);

    // 子供を追加する関数
    void AddChild(Transform2D* child)
    {
        // nullptrでなければ追加
        if (child) m_children.insert(child);
    }
    // 子供を削除する関数
    void RemoveChild(Transform2D* child)
    {
        // 存在するポインタならリストから削除
        if (m_children.find(child) != m_children.end()) m_children.erase(child);
    }
    // 子供を全て解放する関数
    void RemoveChildren();

    // キャッシュを更新する関数
    void UpdateCache() const;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    const DirectX::SimpleMath::Vector2& GetLocalPosition() const { return m_localPosition; }
    const float GetLocalRotation() const { return m_localRotation; }
    const DirectX::SimpleMath::Vector2& GetLocalScale() const { return m_localScale; }

    const DirectX::SimpleMath::Vector2 GetWorldPosition() const;
    const float GetWorldRotation() const ;
    const DirectX::SimpleMath::Vector2 GetWorldScale() const ;

    // 上方向 (Y軸)
    DirectX::SimpleMath::Vector2 GetUp() const 
    {
        float rot = GetWorldRotation();
        return { -sinf(rot), cosf(rot) };
    }

    // 右方向 (X軸)
    DirectX::SimpleMath::Vector2 GetRight() const 
    {
        float rot = GetWorldRotation();
        return { cosf(rot), sinf(rot) };
    }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLocalPosition(const DirectX::SimpleMath::Vector2& pos) 
    {
        m_localPosition = pos; 
        SetDirty();
    }
    void SetLocalRotation(float rot) 
    {
        m_localRotation = rot; 
        SetDirty();
    }
    void SetLocalScale(const DirectX::SimpleMath::Vector2& scale) 
    {
        m_localScale = scale; 
        SetDirty();
    }

    void SetWorldPosition(const DirectX::SimpleMath::Vector2 pos);

    //-----------------------------------------------------
    // 補助関数
    //-----------------------------------------------------
    void AddLocalPosition(const DirectX::SimpleMath::Vector2& value)
    {
        m_localPosition += value;
        SetDirty();
    }
    void AddLocalRotation(float rot)
    {
        m_localRotation += rot;
        SetDirty();
    }
    void AddWorldPosition(const DirectX::SimpleMath::Vector2 value)
    {
        SetWorldPosition(GetWorldPosition() + value);
        SetDirty();
    }
    // キャッシュの値を増やす関数
    inline void AddCache(const DirectX::SimpleMath::Vector2& value)
    {    
        m_addCache += value; 
        m_changeCache = true;
    };

    // キャッシュの値を反映させる関数
    void ReflectCache()
    {
        if (!m_changeCache) return; 

        AddWorldPosition(m_addCache);
        m_addCache = { 0, 0, 0 };
        m_changeCache = false;
    }

    // 自身のローカル座標からワールド座標系へ変換する関数
    DirectX::SimpleMath::Vector2 TransformForWorld(DirectX::SimpleMath::Vector2 point) const;

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    // Dirtyフラグをセットする関数
    void SetDirty();
};