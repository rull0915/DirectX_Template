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
#include "GameLib/GameMath/GameMath.h"

using namespace DirectX::SimpleMath;

#define EPSILON 0.0001f

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
		case ColliderType2D::Box:
			return CheckHit2D(static_cast<CircleCollider2D*>(col1), static_cast<BoxCollider2D*>(col2), info);
		default:
			break;
		}
		break;
	case ColliderType2D::Box:
		switch (col2->GetType())
		{
		case ColliderType2D::Circle:
		{
			bool hit = CheckHit2D(static_cast<CircleCollider2D*>(col2), static_cast<BoxCollider2D*>(col1), info);
			if(hit && info) *info = info->Inverse();
			return hit;
		}
		case ColliderType2D::Box:
			return CheckHit2D(static_cast<BoxCollider2D*>(col1), static_cast<BoxCollider2D*>(col2), info);
		default:
			break;
		}
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

		// 0除算対策
		if (len > EPSILON)
		{
			info->hitDir = (centerB - centerA) / len;
		}
		else
		{
			info->hitDir = Vector2(1, 0); // 適当な安全方向
		}

		info->hitLen = (radiusA + radiusB) - len;
	}

	return hit;
}

bool CheckHit2D(CircleCollider2D* colA, CapsuleCollider2D* colB, HitInfomation2D* info)
{


	return false;
}

bool CheckHit2D(CircleCollider2D* colA, BoxCollider2D* colB, HitInfomation2D* info)
{
	// 情報の取得
	DirectX::SimpleMath::Vector2 posA = colA->GetWorldCenterPos(), posB = colB->GetWorldCenterPos();
	float radius = colA->GetRadius();
	DirectX::SimpleMath::Vector2 xAxis = colB->GetXAxis(), yAxis = colB->GetYAxis();
	DirectX::SimpleMath::Vector2 halfSize = colB->GetHalfSize();

	// ボックスから見た円への相対ベクトル
	Vector2 diff = posA - posB;

	// 球をボックスのローカル座標に変換
	DirectX::SimpleMath::Vector2 localCenter = {
			diff.Dot(xAxis), diff.Dot(yAxis)
	};

	// ローカル座標系での最近点を求める
	DirectX::SimpleMath::Vector2 localNear =
	{
		MyMath::Clamp(localCenter.x, -halfSize.x, halfSize.x),
		MyMath::Clamp(localCenter.y, -halfSize.y, halfSize.y),
	};

	// 半径と比較
	bool hit = (localCenter - localNear).LengthSquared() <= (radius * radius);

	// 衝突情報の計算
	if (hit && info)
	{
		info->own = colA->GetOwn();
		info->target = colB->GetOwn();

		// 最近点のワールド座標を求める
		Vector2 worldNear = posB + (xAxis * localNear.x) + (yAxis * localNear.y);
		
		DirectX::SimpleMath::Vector2 hitVec = (worldNear - posA);

		float len = hitVec.Length();

		if (len > EPSILON)
		{
			info->hitDir = hitVec / len;
		}
		else
		{
			info->hitDir = Vector2(1, 0); // 適当な安全方向
		}
		info->hitLen = radius - len;
	}

	return hit;
}

bool CheckHit2D(BoxCollider2D* colA, BoxCollider2D* colB, HitInfomation2D* info)
{
	// 情報の取得
	// OBBの各軸2本×2の4本の分離軸に対して離れているか確認

	// 各軸を取得
	Vector2 naX = colA->GetXAxis(), naY = colA->GetYAxis();
	Vector2 nbX = colB->GetXAxis(), nbY = colB->GetYAxis();

	// 中心点を取得
	Vector2 aCenter = colA->GetWorldCenterPos(), bCenter = colB->GetWorldCenterPos();
	Vector2 interval = aCenter - bCenter;

	// サイズを取得
	Vector2 aSize = colA->GetHalfSize(), bSize = colB->GetHalfSize();

	// 軸をサイズで掛ける
	Vector2 aX = naX * aSize.x, aY = naY * aSize.y;
	Vector2 bX = nbX * bSize.x, bY = nbY * bSize.y;

	// 投影後の長さ格納用変数
	float lA = 0, lB = 0, l = 0;

	// 最小の重なりとその軸
	float minOverLap = FLT_MAX;
	Vector2 minAxis = { 0, 0 };

	// ----- Aの各軸を判定 ----- //

	// X軸
	lA = aSize.x;
	lB = abs(naX.Dot(bX)) + abs(naX.Dot(bY));
	l = abs(naX.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naX;
	}

	// Y軸
	lA = aSize.y;
	lB = abs(naY.Dot(bX)) + abs(naY.Dot(bY));
	l = abs(naY.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naY;
	}

	// ----- Bの各軸を判定 ----- //

	// X軸
	lA = abs(nbX.Dot(aX)) + abs(nbX.Dot(aY));
	lB = bSize.x;
	l = abs(nbX.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = nbX;
	}

	// Y軸
	lA = abs(nbY.Dot(aX)) + abs(nbY.Dot(aY));
	lB = bSize.y;
	l = abs(nbY.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = nbY;
	}

	// 全ての分離軸で衝突していれば

	// 衝突情報が必要な場合
	if (info)
	{
		// minAxisがAからBを指すように調整
		// DotがプラスならすでにBの方向を向いている
		if (minAxis.Dot(interval) > 0.0f)
		{
			minAxis *= -1.0f;
		}
		info->own = colA->GetOwn();
		info->target = colB->GetOwn();

		info->hitDir = minAxis;
		info->hitLen = minOverLap;
	}

	return true;
}

