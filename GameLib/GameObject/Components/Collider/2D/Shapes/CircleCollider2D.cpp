//====================================================//
// ファイル名  : CircleCollider2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/18
//
// 概要        : 2Dの円状のコライダー
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "CircleCollider2D.h"

#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

void CircleCollider2D::UpdateCache() const
{
	Transform* pT = GetTransform();

	// ワールドの拡大率を取得
	DirectX::SimpleMath::Vector3 worldScale = pT->GetWorldScale();

	// ----- 中心座標の更新 ----- //
	DirectX::SimpleMath::Vector3 localCenter3D = { GetLocalCenterPos().x, GetLocalCenterPos().y, 0 };
	DirectX::SimpleMath::Vector3 center3D = DirectX::SimpleMath::Vector3::Transform(localCenter3D, pT->GetWorldMatrix());

	DirectX::SimpleMath::Vector2 center = { center3D.x, center3D.y };
	SetWorldPosition(center);

	// ----- 半径の更新 ----- //
	float max = std::max(worldScale.x, worldScale.y);

	m_worldRadius = max * m_radius;

	// ----- AABBの更新 ----- //
	DirectX::SimpleMath::Vector3 size = { m_worldRadius, m_worldRadius, m_worldRadius };
	SetBoundingBox(AABB2D(center - size, center + size));

	// フラグのリセット
	ResetDirty();
	SetChanged(true);

	ApplyVersion();
}

void CircleCollider2D::DebugDraw(int color) const
{
	// ワールド行列の算出(Rot,Pos)
	DirectX::SimpleMath::Vector2 pos = GetWorldCenterPos();

	float rad = GetRadius();
	MyRenderer::DrawCircle({ pos.x, pos.y, 0 }, { 0, 0, 1 }, rad, 16, color, false);
}
