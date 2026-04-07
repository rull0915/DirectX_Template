#include "pch.h"
#include "SphereCollider.h"

#include "GameLib/MyRenderer.h"

void SphereCollider::UpdateCache() const
{
	// 変化フラグオフなら
	if (!m_isDirty) return;

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
}

void SphereCollider::DebugDraw(int color) const
{
	// ワールド行列の算出(Rot,Pos)
	DirectX::SimpleMath::Vector3 pos = GetWorldCenterPos();
	DirectX::SimpleMath::Quaternion rot = m_pTransform->GetWorldRotation();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rot) * DirectX::SimpleMath::Matrix::CreateTranslation(pos);

	MyRenderer::SetWorld(world);

	float rad = GetRadius();
	MyRenderer::DrawCircle({ 0, 0, 0 }, { 0, 1, 0 }, rad, 32, color, false);
	MyRenderer::DrawCircle({ 0, 0, 0 }, { 1, 0, 0 }, rad, 32, color, false);
	MyRenderer::DrawCircle({ 0, 0, 0 }, { 0, 0, 1 }, rad, 32, color, false);

	MyRenderer::SetWorld(DirectX::SimpleMath::Matrix::Identity);
}
