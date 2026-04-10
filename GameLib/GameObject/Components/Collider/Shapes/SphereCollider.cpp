#include "pch.h"
#include "SphereCollider.h"

#include "GameLib/MyRenderer.h"

void SphereCollider::UpdateCache() const
{
	// ワールドの拡大率を取得
	DirectX::SimpleMath::Vector3 worldScale = m_pTransform->GetWorldScale();

	// ----- 中心座標の更新 ----- //
	m_worldCenterPos = DirectX::SimpleMath::Vector3::Transform(m_localCenterPos, m_pTransform->GetWorldMatrix());

	// ----- 半径の更新 ----- //
	float max = std::max(std::max(worldScale.x, worldScale.y), worldScale.z);

	m_worldRadius = max * m_radius;

	// ----- AABBの更新 ----- //
	DirectX::SimpleMath::Vector3 size = { m_worldRadius, m_worldRadius, m_worldRadius };
	m_boundingBox = AABB(m_worldCenterPos - size, m_worldCenterPos + size);

	// フラグのリセット
	m_isDirty = false;
	m_isChanged = true;
}

void SphereCollider::DebugDraw(int color) const
{
	// ワールド行列の算出(Rot,Pos)
	DirectX::SimpleMath::Vector3 pos = GetWorldCenterPos();
	DirectX::SimpleMath::Quaternion rot = m_pTransform->GetWorldRotation();

	float rad = GetRadius();
	MyRenderer::DrawCircle(pos, m_pTransform->GetUp(), rad, 16, color, false);
	MyRenderer::DrawCircle(pos, m_pTransform->GetRight(), rad, 16, color, false);
	MyRenderer::DrawCircle(pos, m_pTransform->GetForward(), rad, 16, color, false);
}
