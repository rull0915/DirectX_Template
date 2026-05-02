//====================================================//
// ファイル名  : BoxCollider.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/03
//
// 概要        :  ボックスのコライダー
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "BoxCollider.h"

#include <array>

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

void BoxCollider::UpdateCache() const
{
    Transform* pT = GetTransform();

    // ワールドの拡大率を取得
    SimpleMath::Vector3 worldScale = pT->GetWorldScale();

    // ----- 中心座標の更新 ----- //
    DirectX::SimpleMath::Vector3 world = SimpleMath::Vector3::Transform(GetLocalCenterPos(), pT->GetWorldMatrix());
    SetWorldPosition(world);

    // ----- 各軸の更新 ----- //
    m_cache.xAxis = pT->GetRight();
    m_cache.yAxis = pT->GetUp();
    m_cache.zAxis = pT->GetForward();

    // ----- スケールの更新 ----- //
    m_cache.scale = m_localSize * worldScale;

    // ----- AABBの更新 ----- //
    SimpleMath::Matrix worldRotation = SimpleMath::Matrix::CreateFromQuaternion(pT->GetWorldRotation());

    SimpleMath::Vector3 h = m_cache.scale * 0.5f;

    float ex = abs(m_cache.xAxis.x * h.x) + abs(m_cache.yAxis.x * h.y) + abs(m_cache.zAxis.x * h.z);
    float ey = abs(m_cache.xAxis.y * h.x) + abs(m_cache.yAxis.y * h.y) + abs(m_cache.zAxis.y * h.z);
    float ez = abs(m_cache.xAxis.z * h.x) + abs(m_cache.yAxis.z * h.y) + abs(m_cache.zAxis.z * h.z);

    SimpleMath::Vector3 extent(ex, ey, ez);

    // extentからmin maxを計算
    SetBoundingBox(AABB(world - extent, world + extent));

    // ----- ローカル座標行列の更新 ----- //
    m_cache.localMatrix = pT->GetWorldRotationMatrix() * SimpleMath::Matrix::CreateTranslation(world);

    m_cache.localMatrixInverse = SimpleMath::Matrix::CreateTranslation(-world) * pT->GetWorldRotationMatrix().Transpose();

    // フラグのリセット
    ResetDirty();
    SetChanged(true);

    ApplyVersion();
}

void BoxCollider::DebugDraw(Renderer& renderer, int color) const
{
    // ワールド行列の算出(Rot,Pos)
    SimpleMath::Vector3 pos = GetWorldCenterPos();
    SimpleMath::Quaternion rot = GetTransform()->GetWorldRotation();

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
        renderer.Draw().Line(points[edges[i * 2]], points[edges[i * 2 + 1]], color);
    }
}
