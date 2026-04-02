//====================================================//
// ファイル名   : SphereCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : 球状のコライダー
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
class SphereCollider : public BaseCollider
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 中心座標
    DirectX::SimpleMath::Vector3 m_centerPosition;

    // 半径
    float m_radius;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    SphereCollider(GameObject* own, DirectX::SimpleMath::Vector3 pos, float radius)
        : BaseCollider(own, ColliderType::Sphere)
        , m_centerPosition{ pos }
        , m_radius{ radius }
    {
    };

    ~SphereCollider() = default;

    DirectX::SimpleMath::Vector3 GetCenterPos() const { return m_centerPosition; };
    float GetRadius() const { return m_radius; }

    AABB GetBoundingBox() const override
    {
        return AABB(
            m_centerPosition - DirectX::SimpleMath::Vector3{m_radius, m_radius, m_radius}, 
            m_centerPosition + DirectX::SimpleMath::Vector3{m_radius, m_radius, m_radius}
            );
    };
};