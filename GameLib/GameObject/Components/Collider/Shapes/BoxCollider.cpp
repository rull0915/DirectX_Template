#include "pch.h"
#include "BoxCollider.h"

#include "GameLib/MyRenderer.h"

void BoxCollider::UpdateCache() const
{
    // 変化フラグオフなら
    if (!m_isDirty) return;

    // ワールドの拡大率を取得
    DirectX::SimpleMath::Vector3 worldScale = m_pTransform->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    m_worldCenterPos = DirectX::SimpleMath::Vector3::Transform(m_localCenterPos, m_pTransform->GetWorldMatrix());

    // ----- 各軸の更新 ----- //
    m_cache.xAxis = m_pTransform->GetRight();
    m_cache.yAxis = m_pTransform->GetUp();
    m_cache.zAxis = m_pTransform->GetForward();

    // ----- スケールの更新 ----- //
    m_cache.scale = m_localSize * worldScale;

    // ----- AABBの更新 ----- //
    DirectX::SimpleMath::Matrix worldRotation = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_pTransform->GetWorldRotation());

    DirectX::SimpleMath::Vector3 h = m_cache.scale * 0.5f;

    float ex = abs(m_cache.xAxis.x * h.x) + abs(m_cache.yAxis.x * h.y) + abs(m_cache.zAxis.x * h.z);
    float ey = abs(m_cache.xAxis.y * h.x) + abs(m_cache.yAxis.y * h.y) + abs(m_cache.zAxis.y * h.z);
    float ez = abs(m_cache.xAxis.z * h.x) + abs(m_cache.yAxis.z * h.y) + abs(m_cache.zAxis.z * h.z);

    DirectX::SimpleMath::Vector3 extent(ex, ey, ez);

    // extentからmin maxを計算
    m_boundingBox = AABB(m_worldCenterPos - extent, m_worldCenterPos + extent);

    // ----- ローカル座標行列の更新 ----- //
    m_cache.localMatrix = m_pTransform->GetWorldRotationMatrix() * DirectX::SimpleMath::Matrix::CreateTranslation(m_worldCenterPos);

    m_cache.localMatrixInverse = DirectX::SimpleMath::Matrix::CreateTranslation(-m_worldCenterPos) * m_pTransform->GetWorldRotationMatrix().Transpose();

    // フラグのリセット
    m_isDirty = false;
}

void BoxCollider::DebugDraw(int color) const
{
    // ワールド行列の算出(Rot,Pos)
    DirectX::SimpleMath::Vector3 pos = GetWorldCenterPos();
    DirectX::SimpleMath::Quaternion rot = m_pTransform->GetWorldRotation();

    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rot) * DirectX::SimpleMath::Matrix::CreateTranslation(pos);

    MyRenderer::SetWorld(world);

    DirectX::SimpleMath::Vector3 halfSize = GetHalfSize();

    MyRenderer::Draw3DBox(-halfSize, halfSize, color, false);

    MyRenderer::SetWorld(DirectX::SimpleMath::Matrix::Identity);
}
