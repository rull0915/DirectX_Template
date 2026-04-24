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
#include "CheckHitAuxiliary2D.h"

using namespace DirectX::SimpleMath;

//====================================================//
// 関数の実体宣言
//====================================================//

bool CheckHit2D(BaseCollider2D* colA, BaseCollider2D* colB, HitInfomation2D* info)
{
	switch (colA->GetType())
	{
	case ColliderType2D::Circle:
		switch (colB->GetType())
		{
		case ColliderType2D::Circle:
			return CheckHit2D(static_cast<CircleCollider2D*>(colA), static_cast<CircleCollider2D*>(colB), info);
		case ColliderType2D::Box:
			return CheckHit2D(static_cast<CircleCollider2D*>(colA), static_cast<BoxCollider2D*>(colB), info);
		case ColliderType2D::Capsule:
			return CheckHit2D(static_cast<CircleCollider2D*>(colA), static_cast<CapsuleCollider2D*>(colB), info);
		default:
			break;
		}
		break;
	case ColliderType2D::Box:
		switch (colB->GetType())
		{
		case ColliderType2D::Circle:
		{
			bool hit = CheckHit2D(static_cast<CircleCollider2D*>(colB), static_cast<BoxCollider2D*>(colA), info);
			if(hit && info) *info = info->Inverse();
			return hit;
		}
		case ColliderType2D::Box:
			return CheckHit2D(static_cast<BoxCollider2D*>(colA), static_cast<BoxCollider2D*>(colB), info);
		case ColliderType2D::Capsule:
		{
			bool hit = CheckHit2D(static_cast<CapsuleCollider2D*>(colB), static_cast<BoxCollider2D*>(colA), info);
			if (hit && info) *info = info->Inverse();
			return hit;
		}
		default:
			break;
		}
		break;
	case ColliderType2D::Capsule:

		switch (colB->GetType())
		{
		case ColliderType2D::Circle:
		{
			bool hit = CheckHit2D(static_cast<CapsuleCollider2D*>(colB), static_cast<CircleCollider2D*>(colA), info);
			if (hit && info) *info = info->Inverse();
			return hit;
		}		case ColliderType2D::Box:
			return CheckHit2D(static_cast<CapsuleCollider2D*>(colA), static_cast<BoxCollider2D*>(colB), info);
		case ColliderType2D::Capsule:
			return CheckHit2D(static_cast<CapsuleCollider2D*>(colA), static_cast<CapsuleCollider2D*>(colB), info);
		default:
			break;
		}

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
	// 各座標を取得
	Vector2 center = colA->GetWorldCenterPos();
	std::pair<Vector2, Vector2> points = colB->GetPoints();

	// 最近点を求める
	Vector2 nearPoint = ClosestPointOnSegment(points.first, points.second, center);

	// 距離を算出
	float length = (center - nearPoint).LengthSquared();

	// 半径の合計以下かどうかを返す
	float rad1 = colA->GetRadius(), rad2 = colB->GetRadius();

	bool hit = length <= (rad1 + rad2) * (rad1 + rad2);

	// 衝突時
	if (hit && info)
	{
		// 衝突情報の保存
		Vector2 diff = nearPoint - center;
		float dist = diff.Length();

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			info->hitDir = Vector2::UnitY; // 重なりすぎている時は真上に逃がす
		}
		info->hitLen = (rad1 + rad2) - dist;
		info->own = colA->GetOwn();
		info->target = colB->GetOwn();
	}

	return hit;
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

bool CheckHit2D(CapsuleCollider2D* colA, CapsuleCollider2D* colB, HitInfomation2D* info)
{
	// 各座標を取得
	std::pair<Vector2, Vector2> points1 = colA->GetPoints();
	std::pair<Vector2, Vector2> points2 = colB->GetPoints();

	// 最近点を求める
	Vector2 pP, pQ;
	float l = ClosedSegmentToSegment(points1.first, points1.second, points2.first, points2.second, pP, pQ);

	// 半径の合計以下かどうかを返す
	float rad1 = colA->GetRadius(), rad2 = colB->GetRadius();
	bool hit = l <= (rad1 + rad2) * (rad1 + rad2);

	// 衝突時
	if (hit && info)
	{
		// 衝突情報の保存
		Vector2 diff = pQ - pP;
		float dist = diff.Length();

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			info->hitDir = Vector2::UnitY; // 重なりすぎている時は真上に逃がす
		}
		info->hitLen = (rad1 + rad2) - dist;
		info->own = colA->GetOwn();
		info->target = colB->GetOwn();
	}

	return hit;
}

bool CheckHit2D(CapsuleCollider2D* colA, BoxCollider2D* colB, HitInfomation2D* info)
{
	// OBBの各軸3本 カプセルの軸とOBBの軸の外積3本 カプセルの両端からOBBへの線 この8本の分離軸に対して離れているか確認

	// 情報の取得
	std::pair<Vector2, Vector2> worldPoints = colA->GetPoints();
	float radius = colA->GetRadius();

	Vector2 size = colB->GetHalfSize();

	// OBBの軸
	Vector2 naX = colB->GetXAxis(), naY = colB->GetYAxis();
	Vector2 aX = naX * size.x, aY = naY * size.y;

	// 線分ベクトル
	Vector2 lineVec = worldPoints.second - worldPoints.first;
	Vector2 halfLine = lineVec / 2;
	lineVec.Normalize();

	// 中心間のベクトル
	Vector2 interval = colA->GetWorldCenterPos() - colB->GetWorldCenterPos();

	// 投影後の長さ格納用変数
	float lA = 0, lB = 0, l = 0;

	// 最小の重なりとその軸
	float minOverLap = FLT_MAX;
	Vector2 minAxis = { 0, 0 };

	// ----- Aの各軸を判定 ----- //

	// X軸
	lA = size.x;
	lB = abs(naX.Dot(halfLine)) + radius;
	l = abs(naX.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naX;
	}

	// Y軸
	lA = size.y;
	lB = abs(naY.Dot(halfLine)) + radius;
	l = abs(naY.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naY;
	}

	Vector2 centerB = colB->GetWorldCenterPos();

	// ボックスから見た円への相対ベクトル
	Vector2 diffStart = worldPoints.first - centerB;
	Vector2 diffGoal = worldPoints.second - centerB;

	// 球をボックスのローカル座標に変換
	DirectX::SimpleMath::Vector2 localCenterStart = { diffStart.Dot(naX), diffStart.Dot(naY) };
	DirectX::SimpleMath::Vector2 localCenterGoal =  { diffGoal.Dot(naX), diffGoal.Dot(naY) };

	// ローカル座標系での最近点を求める
	DirectX::SimpleMath::Vector2 localNearStart = { MyMath::Clamp(localCenterStart.x, -size.x, size.x),MyMath::Clamp(localCenterStart.y, -size.y, size.y), };
	DirectX::SimpleMath::Vector2 localNearGoal  = { MyMath::Clamp(localCenterGoal.x, -size.x, size.x),MyMath::Clamp(localCenterGoal.y, -size.y, size.y), };

	// ワールド座標系へ戻す
	Vector2 wStart = centerB + (naX * localNearStart.x) + (naY * localNearStart.y);
	Vector2 wGoal  = centerB + (naX * localNearGoal.x) + (naY * localNearGoal.y);

	// ベクトルを作成
	Vector2 vStart = wStart - worldPoints.first, vGoal = wGoal - worldPoints.second;

	// 正規化
	Vector2 nvStart = vStart, nvGoal = vGoal;
	nvStart.Normalize(), nvGoal.Normalize();

	// このベクトルに対して判定

	// Start
	if (vStart.LengthSquared() > 0.0001f) {
		lA = abs(nvStart.Dot(halfLine)) + radius;
		lB = abs(nvStart.Dot(aX)) + abs(nvStart.Dot(aY));
		l = abs(nvStart.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = nvStart;
		}
	}

	// Goal
	if (vGoal.LengthSquared() > 0.0001f) {
		lA = abs(nvGoal.Dot(halfLine)) + radius;
		lB = abs(nvGoal.Dot(aX)) + abs(nvGoal.Dot(aY));
		l = abs(nvGoal.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = nvGoal;
		}
	}

	Vector2 lineNormal = { -lineVec.y, lineVec.x }; // カプセルの法線
	lA = radius; // カプセルの「幅」
	lB = abs(lineNormal.Dot(aX)) + abs(lineNormal.Dot(aY)); // Boxの投影
	l = abs(lineNormal.Dot(interval));

	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = lineNormal;
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

		info->hitDir = minAxis;
		info->hitLen = minOverLap;
		info->own = colA->GetOwn();
		info->target = colB->GetOwn();
	}

	return true;
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

