//====================================================//
// ファイル名  : SphereCollider.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/03
//
// 概要        :  球状のコライダー
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "SphereCollider.h"

//====================================================//
// 関数の実体宣言
//====================================================//

void SphereCollider::UpdateCache() const
{
	Transform* pT = GetTransform();

	// ワールドの拡大率を取得
	DirectX::SimpleMath::Vector3 worldScale = pT->GetWorldScale();

	// ----- 中心座標の更新 ----- //
	DirectX::SimpleMath::Vector3 world = DirectX::SimpleMath::Vector3::Transform(GetLocalCenterPos(), pT->GetWorldMatrix());
	SetWorldPosition(world);

	// ----- 半径の更新 ----- //
	float max = std::max(std::max(worldScale.x, worldScale.y), worldScale.z);

	m_worldRadius = max * m_radius;

	// ----- AABBの更新 ----- //
	DirectX::SimpleMath::Vector3 size = { m_worldRadius, m_worldRadius, m_worldRadius };
	SetBoundingBox(AABB(world - size, world + size));

	// フラグのリセット
	ResetDirty();
	SetChanged(true);
	ApplyVersion();
}

void SphereCollider::DebugDraw(Renderer& renderer, int color) const
{
	// ワールド行列の算出(Rot,Pos)
	Transform* pT = GetTransform();
	DirectX::SimpleMath::Vector3 pos = GetWorldCenterPos();
	DirectX::SimpleMath::Quaternion rot = pT->GetWorldRotation();

	float rad = GetRadius();
	renderer.Draw().Circle(pos, pT->GetUp(), rad, 16, color, false);
	renderer.Draw().Circle(pos, pT->GetRight(), rad, 16, color, false);
	renderer.Draw().Circle(pos, pT->GetForward(), rad, 16, color, false);
}
