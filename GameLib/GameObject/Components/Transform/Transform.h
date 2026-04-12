//====================================================//
// ファイル名   : Transform.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要 :
//
// 更新履歴 :
// 2026/04/02 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseComponent.h"
#include <unordered_set>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class Transform : public BaseComponent
{
public:
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
    static constexpr int TYPE_ID = MAIN_TRANSFORM;
    static constexpr bool IS_MAIN = true;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 情報
    DirectX::SimpleMath::Vector3 m_localPosition;
    DirectX::SimpleMath::Quaternion m_localRotation;
    DirectX::SimpleMath::Vector3 m_localScale;

    // 行列のキャッシュ
    mutable DirectX::SimpleMath::Matrix m_worldMatrix;

    mutable DirectX::SimpleMath::Matrix m_worldPositionMatrix;    // World座標系の移動行列
    mutable DirectX::SimpleMath::Matrix m_worldRotationMatrix;    // World座標系の回転行列
    mutable DirectX::SimpleMath::Matrix m_worldScaleMatrix;       // World座標系の拡縮行列

    mutable bool m_isDirty; // 値が変わったらtrueにするフラグ

    // --- 親子関係 --- //

    Transform* m_pParent;   // 親

    std::unordered_set<Transform*> m_children; // 子

    // 位置の変更をためておく変数
    DirectX::SimpleMath::Vector3 m_addCache;
    bool m_changeCache;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    Transform(GameObject* own);
    ~Transform();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // 親を設定する関数
    void SetParent(Transform* parent);

    // 子供を追加する関数
    void AddChild(Transform* child)
    {
        // nullptrでなければ追加
        if (child) m_children.insert(child);
    }
    // 子供を削除する関数
    void RemoveChild(Transform* child)
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
    const DirectX::SimpleMath::Vector3& GetLocalPosition() const { return m_localPosition; }
    const DirectX::SimpleMath::Quaternion& GetLocalRotation() const { return m_localRotation; }
    const DirectX::SimpleMath::Vector3 GetLocalEulerAngle() const { return m_localRotation.ToEuler(); }
    const DirectX::SimpleMath::Vector3& GetLocalScale() const { return m_localScale; }

    const DirectX::SimpleMath::Vector3 GetWorldPosition() const ;
    const DirectX::SimpleMath::Quaternion GetWorldRotation() const ;
    const DirectX::SimpleMath::Vector3 GetWorldEulerAngle() const ;
    const DirectX::SimpleMath::Vector3 GetWorldScale() const ;

    // ワールド行列を取得する関数
    DirectX::SimpleMath::Matrix& GetWorldMatrix() const;
    DirectX::SimpleMath::Matrix& GetWorldPositionMatrix() const;
    DirectX::SimpleMath::Matrix& GetWorldRotationMatrix() const;
    DirectX::SimpleMath::Matrix& GetWorldScaleMatrix() const;

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLocalPosition(const DirectX::SimpleMath::Vector3& pos) 
    {
        m_localPosition = pos; 
        SetDirty();
    }
    void SetLocalRotation(const DirectX::SimpleMath::Quaternion& rot) 
    {
        m_localRotation = rot; 
        SetDirty();
    }
    void SetLocalEulerAngle(const DirectX::SimpleMath::Vector3& rot) 
    {
        m_localRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rot); 
        SetDirty();
    }
    void SetLocalScale(const DirectX::SimpleMath::Vector3& scale) 
    {
        m_localScale = scale; 
        SetDirty();
    }

    void SetWorldPosition(const DirectX::SimpleMath::Vector3 pos);

    // 補助関数
    void AddLocalPosition(const DirectX::SimpleMath::Vector3& value)
    {
        m_localPosition += value;
        SetDirty();
    }
    void AddLocalEulerAngle(const DirectX::SimpleMath::Vector3& rot)
    {
        DirectX::SimpleMath::Vector3 euler = GetLocalEulerAngle();
        m_localRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(euler + rot);
        SetDirty();
    }
    void AddWorldPosition(const DirectX::SimpleMath::Vector3 value)
    {
        SetWorldPosition(GetWorldPosition() + value);
        SetDirty();
    }
    // キャッシュの値を増やす関数
    inline void AddCache(const DirectX::SimpleMath::Vector3& value)
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

    // 前方向 (Z軸)
    DirectX::SimpleMath::Vector3 GetForward() const {
        auto f = GetWorldMatrix().Forward();
        f.Normalize();
        return f;
    }

    // 上方向 (Y軸)
    DirectX::SimpleMath::Vector3 GetUp() const {
        auto u = GetWorldMatrix().Up();
        u.Normalize();
        return u;
    }

    // 右方向 (X軸)
    DirectX::SimpleMath::Vector3 GetRight() const {
        auto r = GetWorldMatrix().Right();
        r.Normalize();
        return r;
    }
private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    // Dirtyフラグをセットする関数
    void SetDirty();
};