//====================================================//
// ファイル名   : AABBCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : AABBのコライダー
//
// 更新履歴 :
// 2026/03/18 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseCollider.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class AABBCollider : public BaseCollider
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 2点の座標
    DirectX::SimpleMath::Vector3 m_center;
    DirectX::SimpleMath::Vector3 m_size;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    AABBCollider(GameObject* own, DirectX::SimpleMath::Vector3 center, DirectX::SimpleMath::Vector3 size)
        : BaseCollider(own, ColliderType::AABB)
        , m_center{ center }
        , m_size{ size }
    {
    };
    ~AABBCollider() = default;

    DirectX::SimpleMath::Vector3 GetMinPos() const { return DirectX::SimpleMath::Vector3{ m_center - m_size / 2 }; };
    DirectX::SimpleMath::Vector3 GetMaxPos() const { return DirectX::SimpleMath::Vector3{ m_center + m_size / 2 }; };

    AABB GetBoundingBox() const override
    {
        return AABB(GetMinPos(), GetMaxPos());
    };

    void SetCenterPos(DirectX::SimpleMath::Vector3 pos) { m_center = pos; }
    void SetSize(DirectX::SimpleMath::Vector3 size) { m_size = size; }
};