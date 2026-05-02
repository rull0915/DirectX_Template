//====================================================//
// ファイル名  : BoxCollider2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要        :  2Dボックスのコライダー
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "BoxCollider2D.h"

#include <array>

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

void BoxCollider2D::UpdateCache() const
{
    Transform* pT = GetTransform();
    
    // ワールドの拡大率を取得
    SimpleMath::Vector3 worldScale = pT->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    DirectX::SimpleMath::Vector3 localCenter3D = { GetLocalCenterPos().x, GetLocalCenterPos().y, 0 };
    DirectX::SimpleMath::Vector3 center3D = DirectX::SimpleMath::Vector3::Transform(localCenter3D, pT->GetWorldMatrix());

    DirectX::SimpleMath::Vector2 center = { center3D.x, center3D.y };
    SetWorldPosition(center);


    // ----- 各軸の更新 ----- //
    float zAngle = pT->GetWorldEulerAngle().z;
    m_cache.angle = zAngle;
    m_cache.xAxis = { cosf(zAngle), sinf(zAngle) };
    m_cache.yAxis = { -sinf(zAngle), cosf(zAngle) };

    // ----- スケールの更新 ----- //
    m_cache.scale = { m_localSize.x * worldScale.x, m_localSize.y * worldScale.y };

    // ----- AABBの更新 ----- //
    SimpleMath::Vector2 h = m_cache.scale * 0.5f;

    float ex = abs(m_cache.xAxis.x * h.x) + abs(m_cache.yAxis.x * h.y);
    float ey = abs(m_cache.xAxis.y * h.x) + abs(m_cache.yAxis.y * h.y);

    SimpleMath::Vector2 extent(ex, ey);

    // extentからmin maxを計算
    SetBoundingBox(AABB2D(center - extent, center + extent));

    // フラグのリセット
    ResetDirty();
    SetChanged(true);

    ApplyVersion();
}

void BoxCollider2D::DebugDraw(Renderer& renderer, int color) const
{
    // ワールド行列の算出(Rot,Pos)
    SimpleMath::Vector2 pos = GetWorldCenterPos();

    SimpleMath::Vector2 halfSize = GetHalfSize();
    SimpleMath::Vector2 xA = GetXAxis();
    SimpleMath::Vector2 yA = GetYAxis();

    // PointList
    std::array<SimpleMath::Vector2, 4> points =
    {
        pos + (halfSize.x * xA + halfSize.y * yA),
        pos + (halfSize.x * xA + -halfSize.y * yA),
        pos + (-halfSize.x * xA + -halfSize.y * yA),
        pos + (-halfSize.x * xA + halfSize.y * yA),
    };

    for (int i = 0; i < 4; i++)
    {
        renderer.Draw().Line(
            DirectX::SimpleMath::Vector3{ points[i].x, points[i].y, 0.0f }, 
            DirectX::SimpleMath::Vector3{ points[(i + 1) % 4].x, points[(i + 1) % 4].y, 0.0f }, 
            color);
    }
}
