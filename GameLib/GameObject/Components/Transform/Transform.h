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
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 情報
    DirectX::SimpleMath::Vector3 m_localPosition;
    DirectX::SimpleMath::Quaternion m_localRotation;
    DirectX::SimpleMath::Vector3 m_localScale;

    // 行列のキャッシュ
    DirectX::SimpleMath::Matrix m_worldMatrix;
    bool m_isDirty; // 値が変わったらtrueにするフラグ

    // --- 親子関係 --- //

    Transform* m_pParent;   // 親

    std::unordered_set<Transform*> m_children; // 子

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

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    const DirectX::SimpleMath::Vector3& GetLocalPosition() const { return m_localPosition; }
    const DirectX::SimpleMath::Quaternion& GetLocalRotation() const { return m_localRotation; }
    const DirectX::SimpleMath::Vector3 GetLocalEulerAngle() const { return m_localRotation.ToEuler(); }
    const DirectX::SimpleMath::Vector3& GetLocalScale() const { return m_localScale; }

    const DirectX::SimpleMath::Vector3 GetWorldPosition() ;
    const DirectX::SimpleMath::Quaternion GetWorldRotation() ;
    const DirectX::SimpleMath::Vector3 GetWorldEulerAngle() ;
    const DirectX::SimpleMath::Vector3 GetWorldScale() ;

    // ワールド行列を取得する関数
    DirectX::SimpleMath::Matrix& GetWorldMatrix();

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetLocalPosition(DirectX::SimpleMath::Vector3& pos) 
    {
        m_localPosition = pos; 
        SetDirty();
    }
    void SetLocalRotation(DirectX::SimpleMath::Quaternion& rot) 
    {
        m_localRotation = rot; 
        SetDirty();
    }
    void SetLocalEulerAngle(DirectX::SimpleMath::Vector3& rot) 
    {
        m_localRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rot); 
        SetDirty();
    }
    void SetLocalScale(DirectX::SimpleMath::Vector3& scale) 
    {
        m_localScale = scale; 
        SetDirty();
    }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    // Dirtyフラグをセットする関数
    void SetDirty();
};