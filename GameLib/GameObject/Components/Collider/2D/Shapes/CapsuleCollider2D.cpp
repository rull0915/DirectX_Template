//====================================================//
// ファイル名  : CapsuleCollider2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/03
//
// 概要        :  カプセルのコライダー
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "CapsuleCollider2D.h"

//====================================================//
// 関数の実体宣言
//====================================================//

void CapsuleCollider2D::UpdateCache() const
{
    Transform* pT = GetTransform();

    // ワールドの拡大率を取得
    DirectX::SimpleMath::Vector3 worldScale = pT->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    DirectX::SimpleMath::Vector3 localCenter3D = { GetLocalCenterPos().x, GetLocalCenterPos().y, 0 };
    DirectX::SimpleMath::Vector3 center3D = DirectX::SimpleMath::Vector3::Transform(localCenter3D, pT->GetWorldMatrix());

    DirectX::SimpleMath::Vector2 center = { center3D.x, center3D.y };
    SetWorldPosition(center);

    // ----- ラインのベクトルの更新 ----- //
    {
        DirectX::SimpleMath::Vector2 dir;
        float zAngle = pT->GetWorldEulerAngle().z;

        switch (m_lineDir)
        {
        case AxisType2D::Horizonatl:
            dir = { cosf(zAngle), sinf(zAngle) };
            break;
        case AxisType2D::Vertical:
            dir = { -sinf(zAngle), cosf(zAngle) };
            break;
        default:
            break;
        }
        m_cache.dir = dir;
    }

    // ----- 半径の更新 ----- //
    {
        float ratio = 1;

        switch (m_lineDir)
        {
        case AxisType2D::Horizonatl:
            ratio = worldScale.y; break;
        case AxisType2D::Vertical:
            ratio = worldScale.x; break;
        }

        m_cache.radius = ratio * m_radius;
    }

    // ----- カプセルの高さの更新 ----- //
    {
        float height = 0;

        switch (m_lineDir)
        {
        case AxisType2D::Horizonatl:
            height = worldScale.x * m_capsuleHeight;
            break;
        case AxisType2D::Vertical:
            height = worldScale.y * m_capsuleHeight;
            break;
        default:
            break;
        }

        m_cache.height = height;
    }

    // ----- 線の長さの更新 ----- //
    {
        float result = m_cache.height - m_cache.radius * 2;
        m_cache.lineLength = (result > 0 ? result : 0);
    }

    // ----- 2点を更新する関数 ----- //
    {
        DirectX::SimpleMath::Vector2 dir = m_cache.dir;
        float length = m_cache.lineLength;

        DirectX::SimpleMath::Vector2 line = dir * length;

        m_cache.points = std::make_pair(center - line / 2, center + line / 2);
    }

    // ----- AABBの更新 ----- //
    {
        std::pair<DirectX::SimpleMath::Vector2, DirectX::SimpleMath::Vector2> points = m_cache.points;
        float r = m_cache.radius;

        DirectX::SimpleMath::Vector2 minP = DirectX::SimpleMath::Vector2::Min(points.first, points.second);
        DirectX::SimpleMath::Vector2 maxP = DirectX::SimpleMath::Vector2::Max(points.first, points.second);

        minP -= { r, r };
        maxP += { r, r };

        SetBoundingBox(AABB2D(minP, maxP));
    }

    // フラグのリセット
    ResetDirty();
    SetChanged(true);

    ApplyVersion();
}

void CapsuleCollider2D::DebugDraw(Renderer& renderer, int color) const
{
    float rad = GetRadius();
    float len = GetLineLength();

    DirectX::SimpleMath::Vector2 p1, p2, v;

    p1 = GetPoints().first, p2 = GetPoints().second;
    v = p2 - p1;
    v.Normalize();

    // 中心線分の垂直ベクトルを作成
    DirectX::SimpleMath::Vector2 nV = { -v.y, v.x };
    nV.Normalize();

    if (nV.LengthSquared() < 0.001f)
    {
        renderer.Draw().Circle({ p1 }, { 0, 0, 1 }, rad, 16, color, false);
    }
    else
    {
        // 4点を算出
        DirectX::SimpleMath::Vector2 p3, p4, p5, p6;

        p3 = { p1 + nV * rad };
        p4 = { p1 - nV * rad - v * 0.01f };
        p5 = { p2 + nV * rad };
        p6 = { p2 - nV * rad + v * 0.01f };

        renderer.Draw().Arc({ p1 }, { p3 - p1 }, { p4 - p1 }, 16, rad, color, false);
        renderer.Draw().Arc({ p2 }, { p5 - p2 }, { p6 - p2 }, 16, rad, color, false);

        renderer.Draw().Line({ p3 }, { p5 }, color);
        renderer.Draw().Line({ p4 }, { p6 }, color);
    }
}
