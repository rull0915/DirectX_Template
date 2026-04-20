//====================================================//
// ファイル名  : CheckHit.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要        : 衝突判定の関数をまとめたcpp 
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "CheckHit.h"
#include "CheckHitAuxiliary.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

bool CheckHit(BaseCollider* col1, BaseCollider* col2, HitInfomation* info)
{
	switch (col1->GetType())
	{
		// 1つ目が球の場合
	case ColliderType::Sphere:

		switch (col2->GetType())
		{
			// 2つ目が球の場合
		case ColliderType::Sphere:
			return CheckHit(static_cast<SphereCollider*>(col1), static_cast<SphereCollider*>(col2), info);
		case ColliderType::Capsule:
			return CheckHit(static_cast<SphereCollider*>(col1), static_cast<CapsuleCollider*>(col2), info);
		case ColliderType::Box:
			return CheckHit(static_cast<SphereCollider*>(col1), static_cast<BoxCollider*>(col2), info);
		default:
			break;
		}

		break;

	case ColliderType::Capsule:

		switch (col2->GetType())
		{
		case ColliderType::Sphere:
		{
			bool result = CheckHit(static_cast<SphereCollider*>(col2), static_cast<CapsuleCollider*>(col1), info);
			*info = info->Inverse();
			return result;
		}
		case ColliderType::Capsule:
			return CheckHit(static_cast<CapsuleCollider*>(col1), static_cast<CapsuleCollider*>(col2), info);
		case ColliderType::Box:
			return CheckHit(static_cast<CapsuleCollider*>(col1), static_cast<BoxCollider*>(col2), info);
		default:
			break;
		}

		break;

	case ColliderType::Box:

		switch (col2->GetType())
		{
		case ColliderType::Sphere:
		{
			bool result = CheckHit(static_cast<SphereCollider*>(col2), static_cast<BoxCollider*>(col1), info);
			*info = info->Inverse();
			return result;
		}
		case ColliderType::Capsule:
		{
			bool result = CheckHit(static_cast<CapsuleCollider*>(col2), static_cast<BoxCollider*>(col1), info);
			*info = info->Inverse();
			return result;
		}
		case ColliderType::Box:
			return CheckHit(static_cast<BoxCollider*>(col1), static_cast<BoxCollider*>(col2), info);
		default:
			break;
		}

		break;

	default:
		break;
	}

	return false;
}

bool CheckHit(SphereCollider* col1, SphereCollider* col2, HitInfomation* info)
{
	// 中心座標の距離を算出
	Vector3 center1 = col1->GetWorldCenterPos(), center2 = col2->GetWorldCenterPos();

	float length = (center1 - center2).LengthSquared();

	// 半径の合計以下かどうか
	float rad1 = col1->GetRadius(), rad2 = col2->GetRadius();
	bool hit = length <= (rad1 + rad2) * (rad1 + rad2);

	// 衝突時
	if (hit && info)
	{
		// 衝突情報の保存
		Vector3 diff = center2 - center1;
		float dist = diff.Length();

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			info->hitDir = Vector3::Up; // 重なりすぎている時は真上に逃がす
		}
		info->hitLen = (rad1 + rad2) - dist;
		info->own = col1->GetOwn();
		info->target = col2->GetOwn();
	}

	return hit;
}

bool CheckHit(SphereCollider* col1, CapsuleCollider* col2, HitInfomation* info)
{
	// 各座標を取得
	Vector3 center = col1->GetWorldCenterPos();
	std::pair<Vector3, Vector3> points = col2->GetPoints();

	// 最近点を求める
	Vector3 nearPoint = ClosestPointOnSegment(points.first, points.second, center);

	// 距離を算出
	float length = (center - nearPoint).LengthSquared();

	// 半径の合計以下かどうかを返す
	float rad1 = col1->GetRadius(), rad2 = col2->GetRadius();

	bool hit = length <= (rad1 + rad2) * (rad1 + rad2);

	// 衝突時
	if (hit && info)
	{
		// 衝突情報の保存
		Vector3 diff = nearPoint - center;
		float dist = diff.Length();

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			info->hitDir = Vector3::Up; // 重なりすぎている時は真上に逃がす
		}
		info->hitLen = (rad1 + rad2) - dist;
		info->own = col1->GetOwn();
		info->target = col2->GetOwn();
	}

	return hit;
}

bool CheckHit(SphereCollider* col1, BoxCollider* col2, HitInfomation* info)
{
	// ボックスのワールド行列の逆行列を使って、球の中心をローカル座標へ
	SimpleMath::Matrix worldToLocal = col2->GetLocalMatrixInverse();

	Vector3 worldCenter = col1->GetWorldCenterPos();
	Vector3 localCenter = Vector3::Transform(worldCenter, worldToLocal);

	Vector3 halfSize = col2->GetHalfSize();

	// ボックス内の最近接点を求める
	Vector3 localNear = ClosedPointOnAABB(halfSize, localCenter);

	// 距離の判定（二乗で比較して計算負荷を軽減）
	float distSq = Vector3::DistanceSquared(localCenter, localNear);
	float radius = col1->GetRadius();

	if (distSq > radius * radius) return false;

	// 衝突情報が必要な場合
	if (info) {
		float dist = 0.0f;
		// 球の中心がボックスの「内側」に完全に入っている場合
		if (distSq < 0.0001f) {
			// 最も近い面を探して押し出す
			float dx = halfSize.x - abs(localCenter.x);
			float dy = halfSize.y - abs(localCenter.y);
			float dz = halfSize.z - abs(localCenter.z);

			if (dx < dy && dx < dz) {
				localNear.x = (localCenter.x > 0) ? halfSize.x : -halfSize.x;
				dist = dx;
			}
			else if (dy < dz) {
				localNear.y = (localCenter.y > 0) ? halfSize.y : -halfSize.y;
				dist = dy;
			}
			else {
				localNear.z = (localCenter.z > 0) ? halfSize.z : -halfSize.z;
				dist = dz;
			}
			// 内部にいるので、hitLen は半径 + 面までの距離
			info->hitLen = radius + dist;
		}
		else {
			dist = std::sqrt(distSq);
			info->hitLen = radius - dist;
		}

		Vector3 worldNear = Vector3::Transform(localNear, col2->GetLocalMatrix());
		Vector3 diff = worldNear - worldCenter;

		// 正規化
		if (dist > 0.0001f) info->hitDir = diff / dist;
		else info->hitDir = Vector3::Up;

		info->own = col1->GetOwn();
		info->target = col2->GetOwn();
	}

	return true;
}

bool CheckHit(CapsuleCollider* col1, CapsuleCollider* col2, HitInfomation* info)
{
	// 各座標を取得
	std::pair<Vector3, Vector3> points1 = col1->GetPoints();
	std::pair<Vector3, Vector3> points2 = col2->GetPoints();

	// 最近点を求める
	Vector3 pP, pQ;
	float l = ClosedSegmentToSegment(points1.first, points1.second, points2.first, points2.second, pP, pQ);

	// 半径の合計以下かどうかを返す
	float rad1 = col1->GetRadius(), rad2 = col2->GetRadius();
	bool hit = l <= (rad1 + rad2) * (rad1 + rad2);

	// 衝突時
	if (hit && info)
	{
		// 衝突情報の保存
		Vector3 diff = pQ - pP;
		float dist = diff.Length();

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			info->hitDir = Vector3::Up; // 重なりすぎている時は真上に逃がす
		}
		info->hitLen = (rad1 + rad2) - dist;
		info->own = col1->GetOwn();
		info->target = col2->GetOwn();
	}

	return hit;
}

bool CheckHit(CapsuleCollider* col1, BoxCollider* col2, HitInfomation* info)
{
	// OBBの各軸3本 カプセルの軸とOBBの軸の外積3本 カプセルの両端からOBBへの線 この8本の分離軸に対して離れているか確認

	// 情報の取得
	std::pair<Vector3, Vector3> worldPoints = col1->GetPoints();
	float radius = col1->GetRadius();

	Vector3 size = col2->GetHalfSize();

	// OBBの軸
	Vector3 naX = col2->GetXAxis(), naY = col2->GetYAxis(), naZ = col2->GetZAxis();
	Vector3 aX = naX * size.x, aY = naY * size.y, aZ = naZ * size.z;

	// 線分ベクトル
	Vector3 lineVec = worldPoints.second - worldPoints.first;
	Vector3 halfLine = lineVec / 2;
	lineVec.Normalize();

	// 中心間のベクトル
	Vector3 interval = col1->GetWorldCenterPos() - col2->GetWorldCenterPos();

	// 投影後の長さ格納用変数
	float lA = 0, lB = 0, l = 0;

	// 最小の重なりとその軸
	float minOverLap = FLT_MAX;
	Vector3 minAxis = { 0, 0, 0 };

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

	// Z軸
	lA = size.z;
	lB = abs(naZ.Dot(halfLine)) + radius;
	l = abs(naZ.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naZ;
	}

	// 軸同士の外積

	// X軸
	Vector3 crX = lineVec.Cross(naX);
	if (crX.LengthSquared() > 0.0001f) {
		crX.Normalize();
		lA = radius;
		lB = abs(crX.Dot(aY)) + abs(crX.Dot(aZ));
		l = abs(crX.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crX;
		}
	}

	// Y軸
	Vector3 crY = lineVec.Cross(naY);
	if (crY.LengthSquared() > 0.0001f) {
		crY.Normalize();
		lA = radius;
		lB = abs(crY.Dot(aX)) + abs(crY.Dot(aZ));
		l = abs(crY.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crY;
		}
	}

	// Z軸
	Vector3 crZ = lineVec.Cross(naZ);
	if (crZ.LengthSquared() > 0.0001f) {
		crZ.Normalize();
		lA = radius;
		lB = abs(crZ.Dot(aX)) + abs(crZ.Dot(aY));
		l = abs(crZ.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crZ;
		}
	}

	// 端点からOBBへの最近点を求める
	SimpleMath::Matrix local = col2->GetLocalMatrix();
	SimpleMath::Matrix localInv = col2->GetLocalMatrixInverse();

	// 線分をボックスのローカル座標系に変換
	Vector3 start = Vector3::Transform(worldPoints.first, localInv);
	Vector3 end = Vector3::Transform(worldPoints.second, localInv);

	// ボックスへの最近点を求める
	Vector3 nearStart = ClosedPointOnAABB(size, start), nearGoal = ClosedPointOnAABB(size, end);

	// ワールド座標系へ戻す
	Vector3 wStart = Vector3::Transform(nearStart, local), wGoal = Vector3::Transform(nearGoal, local);

	// ベクトルを作成
	Vector3 vStart = wStart - worldPoints.first, vGoal = wGoal - worldPoints.second;

	// 正規化
	Vector3 nvStart = vStart, nvGoal = vGoal;
	nvStart.Normalize(), nvGoal.Normalize();

	// このベクトルに対して判定

	// Start
	if (vStart.LengthSquared() > 0.0001f) {
		lA = abs(nvStart.Dot(halfLine)) + radius;
		lB = abs(nvStart.Dot(aX)) + abs(nvStart.Dot(aY)) + abs(nvStart.Dot(aZ));
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
		lB = abs(nvGoal.Dot(aX)) + abs(nvGoal.Dot(aY)) + abs(nvGoal.Dot(aZ));
		l = abs(nvGoal.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = nvGoal;
		}
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
		info->own = col1->GetOwn();
		info->target = col2->GetOwn();
	}

	return true;
}

bool CheckHit(BoxCollider* colA, BoxCollider* colB, HitInfomation* info)
{
	// OBBの各軸3本×2, 軸同士の外積3本×3本 この15本の分離軸に対して離れているか確認

	// 各軸を取得
	Vector3 naX = colA->GetXAxis(), naY = colA->GetYAxis(), naZ = colA->GetZAxis();
	Vector3 nbX = colB->GetXAxis(), nbY = colB->GetYAxis(), nbZ = colB->GetZAxis();

	// 中心点を取得
	Vector3 aCenter = colA->GetWorldCenterPos(), bCenter = colB->GetWorldCenterPos();
	Vector3 interval = aCenter - bCenter;

	// サイズを取得
	Vector3 aSize = colA->GetHalfSize(), bSize = colB->GetHalfSize();

	// 軸をサイズで掛ける
	Vector3 aX = naX * aSize.x, aY = naY * aSize.y, aZ = naZ * aSize.z;
	Vector3 bX = nbX * bSize.x, bY = nbY * bSize.y, bZ = nbZ * bSize.z;

	// 投影後の長さ格納用変数
	float lA = 0, lB = 0, l = 0;

	// 最小の重なりとその軸
	float minOverLap = FLT_MAX;
	Vector3 minAxis = { 0, 0, 0 };

	// ----- Aの各軸を判定 ----- //

	// X軸
	lA = aSize.x;
	lB = LengthSegmentOnSeparateAxis(naX, bX, bY, bZ);
	l = abs(naX.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naX;
	}

	// Y軸
	lA = aSize.y;
	lB = LengthSegmentOnSeparateAxis(naY, bX, bY, bZ);
	l = abs(naY.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naY;
	}

	// Z軸
	lA = aSize.z;
	lB = LengthSegmentOnSeparateAxis(naZ, bX, bY, bZ);
	l = abs(naZ.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = naZ;
	}

	// ----- Bの各軸を判定 ----- //

	// X軸
	lA = LengthSegmentOnSeparateAxis(nbX, aX, aY, aZ);
	lB = bSize.x;
	l = abs(nbX.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = nbX;
	}

	// Y軸
	lA = LengthSegmentOnSeparateAxis(nbY, aX, aY, aZ);
	lB = bSize.y;
	l = abs(nbY.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = nbY;
	}

	// Z軸
	lA = LengthSegmentOnSeparateAxis(nbZ, aX, aY, aZ);
	lB = bSize.z;
	l = abs(nbZ.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = nbZ;
	}

	// それぞれの外積を判定

	// XとX
	Vector3 crXX = naX.Cross(nbX);
	if (crXX.LengthSquared() > 0.0001f) {
		crXX.Normalize();
		lA = abs(crXX.Dot(aY)) + abs(crXX.Dot(aZ));
		lB = abs(crXX.Dot(bY)) + abs(crXX.Dot(bZ));
		l = abs(crXX.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crXX;
		}
	}

	// XとY
	Vector3 crXY = naX.Cross(nbY);
	if (crXY.LengthSquared() > 0.0001f) {
		crXY.Normalize();
		lA = abs(crXY.Dot(aY)) + abs(crXY.Dot(aZ));
		lB = abs(crXY.Dot(bX)) + abs(crXY.Dot(bZ));
		l = abs(crXY.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crXY;
		}
	}

	// XとZ
	Vector3 crXZ = naX.Cross(nbZ);
	if (crXZ.LengthSquared() > 0.0001f) {
		crXZ.Normalize();
		lA = abs(crXZ.Dot(aY)) + abs(crXZ.Dot(aZ));
		lB = abs(crXZ.Dot(bX)) + abs(crXZ.Dot(bY));
		l = abs(crXZ.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crXZ;
		}
	}

	// YとX
	Vector3 crYX = naY.Cross(nbX);
	if (crYX.LengthSquared() > 0.0001f) {
		crYX.Normalize();
		lA = abs(crYX.Dot(aX)) + abs(crYX.Dot(aZ));
		lB = abs(crYX.Dot(bY)) + abs(crYX.Dot(bZ));
		l = abs(crYX.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crYX;
		}
	}

	// YとY
	Vector3 crYY = naY.Cross(nbY);
	if (crYY.LengthSquared() > 0.0001f) {
		crYY.Normalize();
		lA = abs(crYY.Dot(aX)) + abs(crYY.Dot(aZ));
		lB = abs(crYY.Dot(bX)) + abs(crYY.Dot(bZ));
		l = abs(crYY.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crYY;
		}
	}

	// YとZ
	Vector3 crYZ = naY.Cross(nbZ);
	if (crYZ.LengthSquared() > 0.0001f) {
		crYZ.Normalize();
		lA = abs(crYZ.Dot(aX)) + abs(crYZ.Dot(aZ));
		lB = abs(crYZ.Dot(bX)) + abs(crYZ.Dot(bY));
		l = abs(crYZ.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crYZ;
		}
	}

	// ZとX
	Vector3 crZX = naZ.Cross(nbX);
	if (crZX.LengthSquared() > 0.0001f) {
		crZX.Normalize();
		lA = abs(crZX.Dot(aX)) + abs(crZX.Dot(aY));
		lB = abs(crZX.Dot(bY)) + abs(crZX.Dot(bZ));
		l = abs(crZX.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crZX;
		}
	}

	// ZとY
	Vector3 crZY = naZ.Cross(nbY);
	if (crZY.LengthSquared() > 0.0001f) {
		crZY.Normalize();
		lA = abs(crZY.Dot(aX)) + abs(crZY.Dot(aY));
		lB = abs(crZY.Dot(bX)) + abs(crZY.Dot(bZ));
		l = abs(crZY.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crZY;
		}
	}

	// ZとZ
	Vector3 crZZ = naZ.Cross(nbZ);
	if (crZZ.LengthSquared() > 0.0001f) {
		crZZ.Normalize();
		lA = abs(crZZ.Dot(aX)) + abs(crZZ.Dot(aY));
		lB = abs(crZZ.Dot(bX)) + abs(crZZ.Dot(bY));
		l = abs(crZZ.Dot(interval));
		if (lA + lB < l) return false;
		if ((lA + lB) - l < minOverLap)
		{
			minOverLap = (lA + lB) - l;
			minAxis = crZZ;
		}
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

bool CheckHit(LineCollider* col1, SphereCollider* col2)
{
	// 情報の取得
	Vector3 center = col2->GetWorldCenterPos();

	Vector3 start = col1->GetStart(), end = col1->GetEnd();

	// 球と線分の距離を取得する

	// 線分上の最近点を求める
	Vector3 nearPoint = ClosestPointOnSegment(start, end, center);

	float lenSq = (center - nearPoint).LengthSquared();

	float rad = col2->GetRadius();
	return lenSq <= rad * rad;
}

bool CheckHit(LineCollider* col1, CapsuleCollider* col2)
{
	// 情報の取得
	Vector3 startA = col1->GetStart(), endA = col1->GetEnd();
	Vector3 startB = col2->GetPoints().first, endB = col2->GetPoints().second;

	// 球と線分の距離を取得する

	// 線分上の最近点を求める
	Vector3 outP, outQ;
	float lenSq = ClosedSegmentToSegment(startA, endA, startB, endB, outP, outQ);

	float rad = col2->GetRadius();
	return lenSq <= rad * rad;
}

bool CheckHit(LineCollider* col1, BoxCollider* col2)
{
	// 情報の取得
	Vector3 worldStart = col1->GetStart(), worldEnd = col1->GetEnd();

	Vector3 size = col2->GetHalfSize();

	// ボックスのワールド行列の逆行列を使って、球の中心をローカル座標へ
	SimpleMath::Matrix worldToLocal = col2->GetLocalMatrixInverse();

	Vector3 locaStart = Vector3::Transform(worldStart, worldToLocal), localEnd = Vector3::Transform(worldStart, worldToLocal);

	Vector3 halfSize = col2->GetHalfSize();

	// ローカル座標系で、線分をボックス内にクランプする

	return false;
}
