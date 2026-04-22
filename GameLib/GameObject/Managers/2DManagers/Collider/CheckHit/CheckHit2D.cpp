//====================================================//
// ファイル名  : CheckHit2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要        : 2D衝突判定の関数をまとめたcpp 
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "CheckHit2D.h"

using namespace DirectX::SimpleMath;

//====================================================//
// 関数の実体宣言
//====================================================//

bool CheckHit2D(BaseCollider2D* col1, BaseCollider2D* col2, HitInfomation2D* info)
{
	switch (col1->GetType())
	{
	case ColliderType2D::Circle:
		switch (col2->GetType())
		{
		case ColliderType2D::Circle:
			return CheckHit2D(static_cast<CircleCollider2D*>(col1), static_cast<CircleCollider2D*>(col2), info);
		default:
			break;
		}
		break;
	case ColliderType2D::Box:
		break;
	case ColliderType2D::Capsule:
		break;
	default:
		break;
	}
}

bool CheckHit2D(CircleCollider2D* colA, CircleCollider2D* colB, HitInfomation2D* info)
{
	// 情報の取得
	DirectX::SimpleMath::Vector2 centerA = colA->GetWorldCenterPos(), centerB = colB->GetWorldCenterPos();
	float radiusA = colA->GetRadius(), radiusB = colB->GetRadius();

	float lenSq = (centerB - centerA).LengthSquared();

	// 長さの2乗で比較
	bool hit = lenSq <= (radiusA + radiusB) * (radiusA + radiusB);

	// 衝突情報の計算
	if (hit && info)
	{
		info->own = colA->GetOwn();
		info->target = colB->GetOwn();

		float len = (centerB - centerA).Length();

		info->hitDir = (centerB - centerA) / len;
		info->hitLen = len;
	}

	return hit;
}

