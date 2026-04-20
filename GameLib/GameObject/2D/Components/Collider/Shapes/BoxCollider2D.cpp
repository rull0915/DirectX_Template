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

#include "GameLib/MyRenderer.h"
#include <array>

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

void BoxCollider2D::UpdateCache() const
{
    if (!m_isDirty) return;

    // ワールドの拡大率を取得
    SimpleMath::Vector2 worldScale = m_pTransform->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    m_worldCenterPos = m_pTransform->TransformForWorld(m_localCenterPos);

    // ----- 各軸の更新 ----- //
    m_cache.xAxis = m_pTransform->GetRight();
    m_cache.yAxis = m_pTransform->GetUp();

    // ----- スケールの更新 ----- //
    m_cache.scale = m_localSize * worldScale;

    // ----- AABBの更新 ----- //
    SimpleMath::Vector2 h = m_cache.scale * 0.5f;

    float ex = abs(m_cache.xAxis.x * h.x) + abs(m_cache.yAxis.x * h.y);
    float ey = abs(m_cache.xAxis.y * h.x) + abs(m_cache.yAxis.y * h.y);

    SimpleMath::Vector2 extent(ex, ey);

    // extentからmin maxを計算
    m_boundingBox = AABB2D(m_worldCenterPos - extent, m_worldCenterPos + extent);

    // フラグのリセット
    m_isDirty = false;
    m_isChanged = true;
}

void BoxCollider2D::DebugDraw(int color) const
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
        MyRenderer::DrawLine(
            DirectX::SimpleMath::Vector3{ points[i].x, points[i].y, 0.0f }, 
            DirectX::SimpleMath::Vector3{ points[(i + 1) % 4].x, points[(i + 1) % 4].y, 0.0f }, 
            color);
    }
}
