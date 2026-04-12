#include "pch.h"
#include "BoxCollider.h"

#include "GameLib/MyRenderer.h"

using namespace DirectX;

void BoxCollider::UpdateCache() const
{
    if (!m_isDirty) return;

    // ワールドの拡大率を取得
    SimpleMath::Vector3 worldScale = m_pTransform->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    m_worldCenterPos = SimpleMath::Vector3::Transform(m_localCenterPos, m_pTransform->GetWorldMatrix());

    // ----- 各軸の更新 ----- //
    m_cache.xAxis = m_pTransform->GetRight();
    m_cache.yAxis = m_pTransform->GetUp();
    m_cache.zAxis = m_pTransform->GetForward();

    // ----- スケールの更新 ----- //
    m_cache.scale = m_localSize * worldScale;

    // ----- AABBの更新 ----- //
    SimpleMath::Matrix worldRotation = SimpleMath::Matrix::CreateFromQuaternion(m_pTransform->GetWorldRotation());

    SimpleMath::Vector3 h = m_cache.scale * 0.5f;

    float ex = abs(m_cache.xAxis.x * h.x) + abs(m_cache.yAxis.x * h.y) + abs(m_cache.zAxis.x * h.z);
    float ey = abs(m_cache.xAxis.y * h.x) + abs(m_cache.yAxis.y * h.y) + abs(m_cache.zAxis.y * h.z);
    float ez = abs(m_cache.xAxis.z * h.x) + abs(m_cache.yAxis.z * h.y) + abs(m_cache.zAxis.z * h.z);

    SimpleMath::Vector3 extent(ex, ey, ez);

    // extentからmin maxを計算
    m_boundingBox = AABB(m_worldCenterPos - extent, m_worldCenterPos + extent);

    // ----- ローカル座標行列の更新 ----- //
    m_cache.localMatrix = m_pTransform->GetWorldRotationMatrix() * SimpleMath::Matrix::CreateTranslation(m_worldCenterPos);

    m_cache.localMatrixInverse = SimpleMath::Matrix::CreateTranslation(-m_worldCenterPos) * m_pTransform->GetWorldRotationMatrix().Transpose();

    // フラグのリセット
    m_isDirty = false;
    m_isChanged = true;
}

void BoxCollider::DebugDraw(int color) const
{
    // ワールド行列の算出(Rot,Pos)
    SimpleMath::Vector3 pos = GetWorldCenterPos();
    SimpleMath::Quaternion rot = m_pTransform->GetWorldRotation();

    SimpleMath::Matrix world = SimpleMath::Matrix::CreateFromQuaternion(rot) * SimpleMath::Matrix::CreateTranslation(pos);

    SimpleMath::Vector3 halfSize = GetHalfSize();

    // PointList
    std::array<SimpleMath::Vector3, 8> points =
    {
        SimpleMath::Vector3::Transform(SimpleMath::Vector3( halfSize.x,  halfSize.y,  halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3(-halfSize.x,  halfSize.y,  halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3(-halfSize.x,  halfSize.y, -halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3( halfSize.x,  halfSize.y, -halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3( halfSize.x, -halfSize.y,  halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3(-halfSize.x, -halfSize.y,  halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3(-halfSize.x, -halfSize.y, -halfSize.z), world),
        SimpleMath::Vector3::Transform(SimpleMath::Vector3( halfSize.x, -halfSize.y, -halfSize.z), world),
    };

    std::array<int, 12 * 2> edges =
    {
        0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4
    };
    
    for (int i = 0; i < 12; i++)
    {
        MyRenderer::DrawLine(points[edges[i * 2]], points[edges[i * 2 + 1]], color);
    }
}
