#include "pch.h"
#include "CapsuleCollider.h"

#include "GameLib/MyRenderer.h"

void CapsuleCollider::UpdateCache() const
{
    // ワールドの拡大率を取得
    DirectX::SimpleMath::Vector3 worldScale = m_pTransform->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    m_worldCenterPos = DirectX::SimpleMath::Vector3::Transform(m_localCenterPos, m_pTransform->GetWorldMatrix());

    // ----- ラインのベクトルの更新 ----- //
    {
        DirectX::SimpleMath::Vector3 dir;

        switch (m_lineDir)
        {
        case AxisType::X:
            dir = m_pTransform->GetRight();
            break;
        case AxisType::Y:
            dir = m_pTransform->GetUp();
            break;
        case AxisType::Z:
            dir = m_pTransform->GetForward();
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
        case AxisType::X:
            ratio = std::max(worldScale.y, worldScale.z); break;
        case AxisType::Y:
            ratio = std::max(worldScale.x, worldScale.z); break;
        case AxisType::Z:
            ratio = std::max(worldScale.x, worldScale.y); break;
        }

        m_cache.radius = ratio * m_radius;
    }

    // ----- カプセルの高さの更新 ----- //
    {
        float height = 0;

        switch (m_lineDir)
        {
        case AxisType::X:
            height = worldScale.x * m_capsuleHeight;
            break;
        case AxisType::Y:
            height = worldScale.y * m_capsuleHeight;
            break;
        case AxisType::Z:
            height = worldScale.z * m_capsuleHeight;
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
        DirectX::SimpleMath::Vector3 center = m_worldCenterPos;
        DirectX::SimpleMath::Vector3 dir = m_cache.dir;
        float length = m_cache.lineLength;

        DirectX::SimpleMath::Vector3 line = dir * length;

        m_cache.points = std::make_pair(center - line / 2, center + line / 2);
    }

    // ----- AABBの更新 ----- //
    {
        std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3> points = m_cache.points;
        float r = m_cache.radius;

        DirectX::SimpleMath::Vector3 minP = DirectX::SimpleMath::Vector3::Min(points.first, points.second);
        DirectX::SimpleMath::Vector3 maxP = DirectX::SimpleMath::Vector3::Max(points.first, points.second);

        minP -= { r, r, r };
        maxP += { r, r, r };

        m_boundingBox = AABB(minP, maxP);
    }

    // フラグのリセット
    m_isDirty = false;
    m_isChanged = true;
}

void CapsuleCollider::DebugDraw(int color) const
{
    float rad = GetRadius();
    float len = GetLineLength();

    DirectX::SimpleMath::Vector3 p1, p2, v;

    switch (m_lineDir)
    {
    case AxisType::X:
        p1 = { -len * 0.5f, 0, 0 }; p2 = { len * 0.5f, 0, 0 }; v = { 1, 0, 0 }; break;
    case AxisType::Y:
        p1 = { 0, -len * 0.5f, 0 }; p2 = { 0, len * 0.5f, 0 }; v = { 0, 1, 0 }; break;
    case AxisType::Z:
        p1 = { 0, 0, -len * 0.5f }; p2 = { 0, 0, len * 0.5f }; v = { 0, 0, 1 }; break;
    default:
        break;
    }

    // ワールド行列の算出(Rot,Pos)
    DirectX::SimpleMath::Vector3 pos = GetWorldCenterPos();
    DirectX::SimpleMath::Quaternion rot = m_pTransform->GetWorldRotation();

    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rot) * DirectX::SimpleMath::Matrix::CreateTranslation(pos);

    MyRenderer::SetWorld(world);

    // 4つのベクトルを作成
    DirectX::SimpleMath::Vector3 v1, v2, v3, v4, v5, v6, v7, v8;

    // 2つの半円の描画
    switch (m_lineDir)
    {
    case AxisType::X:
        v1 = { 0, 1, 0 }; v2 = { 0.01f, -1, 0 }; v3 = { 0, 0, 1 }; v4 = { 0.01f, 0, -1 };
        v5 = { 0, 1, 0 }; v6 = { -0.01f, -1, 0 }; v7 = { 0, 0, 1 }; v8 = { -0.01f, 0, -1 };
        break;
    case AxisType::Y:
        v1 = { 1, 0, 0 }; v2 = { -1, 0.01f, 0 }; v3 = { 0, 0, 1 }; v4 = { 0, 0.01f, -1 };
        v5 = { 1, 0, 0 }; v6 = { -1, -0.01f, 0 }; v7 = { 0, 0, 1 }; v8 = { 0, -0.01f, -1 };
        break;
    case AxisType::Z:
        v1 = { 0, 1, 0 }; v2 = { 0, -1, 0.01f }; v3 = { 1, 0, 0 }; v4 = { -1, 0, 0.01f };
        v5 = { 0, 1, 0 }; v6 = { 0, -1, -0.01f }; v7 = { 1, 0, 0 }; v8 = { -1, 0, -0.01f };
        break;
    default:
        break;
    }

    v1.Normalize(); v2.Normalize(); v3.Normalize(); v4.Normalize();

    MyRenderer::DrawArc(p1, v5, v6, rad, color);
    MyRenderer::DrawArc(p1, v7, v8, rad, color);
    MyRenderer::DrawCircle(p1, v, rad, 32, color, false);

    MyRenderer::DrawArc(p2, v1, v2, rad, color);
    MyRenderer::DrawArc(p2, v3, v4, rad, color);
    MyRenderer::DrawCircle(p2, v, rad, 32, color, false);

    MyRenderer::DrawLine(p1 + v1 * rad, p2 + v1 * rad, color);
    MyRenderer::DrawLine(p1 + v2 * rad, p2 + v2 * rad, color);
    MyRenderer::DrawLine(p1 + v3 * rad, p2 + v3 * rad, color);
    MyRenderer::DrawLine(p1 + v4 * rad, p2 + v4 * rad, color);

    MyRenderer::SetWorld(DirectX::SimpleMath::Matrix::Identity);
}
