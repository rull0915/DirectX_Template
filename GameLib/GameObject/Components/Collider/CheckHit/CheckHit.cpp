#include "CheckHit.h"
#include "CheckHitAuxiliary.h"

using namespace DirectX;

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
	// ※可能であれば col2->GetWorldMatrixInv() のようにキャッシュしたものを使いたい
	SimpleMath::Matrix worldToLocal = col2->GetLocalMatrix().Invert();

	Vector3 worldCenter = col1->GetWorldCenterPos();
	Vector3 localCenter = Vector3::Transform(worldCenter, worldToLocal);

	Vector3 halfSize = col2->GetHalfSize();

	// ボックス内の最近接点を求める（クランプ）
	Vector3 localNear = {
		MyMath::Clamp(localCenter.x, -halfSize.x, halfSize.x),
		MyMath::Clamp(localCenter.y, -halfSize.y, halfSize.y),
		MyMath::Clamp(localCenter.z, -halfSize.z, halfSize.z)
	};

	// 距離の判定（二乗で比較して計算負荷を軽減）
	float distSq = Vector3::DistanceSquared(localCenter, localNear);
	float radius = col1->GetRadius();

	if (distSq > radius * radius) return false;

	// 衝突時の情報作成
	if (info)
	{
		// ローカルの最近接点をワールドに戻す
		Vector3 worldNear = Vector3::Transform(localNear, col2->GetLocalMatrix());

		Vector3 diff = worldNear - worldCenter;
		float dist = sqrtf(distSq);

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			// 中心が完全に重なった、または近すぎる場合
			info->hitDir = Vector3::Up;
		}

		info->hitLen = radius - dist;
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
	// 情報の取得
	std::pair<Vector3, Vector3> worldPoints = col1->GetPoints();
	float radius = col1->GetRadius();

	Vector3 size = col2->GetHalfSize();
	SimpleMath::Matrix local = col2->GetLocalMatrix();

	// 線分をボックスのローカル座標系に変換
	Vector3 start = Vector3::Transform(worldPoints.first, local.Invert());
	Vector3 end = Vector3::Transform(worldPoints.second, local.Invert());

	Vector3 radVec = { radius, radius, radius };

	// 拡大したAABBの作成
	Vector3 min = - size - radVec, max = size + radVec;
	Vector3 borderMin = -size, borderMax = size;

	// 線分と拡大AABBとの衝突範囲を算出

	Vector3 dir = end - start;
	 
	// どのエリアに衝突しているかを調べる
	float tx[4] = {		// x成分
		(min.x - start.x) / dir.x,
		(borderMin.x - start.x) / dir.x,
		(borderMax.x - start.x) / dir.x,
		(max.x - start.x) / dir.x};
	// x成分の傾きが0付近だった場合
	if (abs(dir.x) < 0.00001f){
		if (start.x < min.x || start.x > max.x){
			tx[0] = -1, tx[1] = -1, tx[2] = -1, tx[3] = -1;
		}
		else {
			if (start.x < borderMin.x)		{ tx[0] = 0, tx[1] = 1, tx[2] = 1, tx[3] = 1; }
			else if (start.x < borderMax.x)	{ tx[0] = 0, tx[1] = 0, tx[2] = 1, tx[3] = 1; }
			else							{ tx[0] = 0, tx[1] = 0, tx[2] = 0, tx[3] = 1; }
		}
	}
	float ty[4] = {		// y成分
		(min.y - start.y) / dir.y,
		(borderMin.y - start.y) / dir.y,
		(borderMax.y - start.y) / dir.y,
		(max.y - start.y) / dir.y};
	// y成分の傾きが0付近だった場合
	if (abs(dir.y) < 0.00001f) {
		if (start.y < min.y || start.y > max.y) {
			ty[0] = -1, ty[1] = -1, ty[2] = -1, ty[3] = -1;
		}
		else {
			if (start.y < borderMin.y) { ty[0] = 0, ty[1] = 1, ty[2] = 1, ty[3] = 1; }
			else if (start.y < borderMax.y) { ty[0] = 0, ty[1] = 0, ty[2] = 1, ty[3] = 1; }
			else { ty[0] = 0, ty[1] = 0, ty[2] = 0, ty[3] = 1; }
		}
	}
	float tz[4] = {		// z成分
		(min.z - start.z) / dir.z,
		(borderMin.z - start.z) / dir.z,
		(borderMax.z - start.z) / dir.z,
		(max.z - start.z) / dir.z};
	// z成分の傾きが0付近だった場合
	if (abs(dir.z) < 0.00001f) {
		if (start.z < min.z || start.z > max.z) {
			tz[0] = -1, tz[1] = -1, tz[2] = -1, tz[3] = -1;
		}
		else {
			if (start.z < borderMin.z) { tz[0] = 0, tz[1] = 1, tz[2] = 1, tz[3] = 1; }
			else if (start.z < borderMax.z) { tz[0] = 0, tz[1] = 0, tz[2] = 1, tz[3] = 1; }
			else { tz[0] = 0, tz[1] = 0, tz[2] = 0, tz[3] = 1; }
		}
	}

	using namespace MyMath;

	// 最短ベクトル
	float minLenSq = FLT_MAX; // 距離比較用
	Vector3 pS, pB;

	bool hit = false;

	// 共通部分を調べる
	for (int ix = 0; ix < 3; ++ix) {
		Range rangeX = CommonPart(Range(tx[ix], tx[ix + 1], true), Range(0, 1));
		if (rangeX.IsNull()) continue; // Xの時点で外れていればスキップ

		for (int iy = 0; iy < 3; ++iy) {
			Range rangeY = CommonPart(rangeX, Range(ty[iy], ty[iy + 1], true));
			if (rangeY.IsNull()) continue; // XYの時点で外れていればスキップ

			for (int iz = 0; iz < 3; ++iz) {
				Range rangeZ = CommonPart(rangeY, Range(tz[iz], tz[iz + 1], true));
				if (rangeZ.IsNull()) continue;

				// x, y, zでの共通範囲が存在した場合

				// エリア別の衝突チェック
				Vector3 outSeg, outBox;
				if (CheckArea(ix, iy, iz, start, end, rangeZ.Min(), rangeZ.Max(), size, radius, outSeg, outBox)) hit = true;
				if (hit && !info) return true;

				if (hit) {
					Vector3 currentVec = (outBox - outSeg);
					float currentLenSq = currentVec.LengthSquared();
					if (currentLenSq < minLenSq) {
						minLenSq = currentLenSq;
						pS = outSeg, pB = outBox;
					}
				}
			}
		}
	}

	// 衝突時
	if (hit && info)
	{
		// ワールド座標系に変換
		Vector3 wpS = Vector3::Transform(pS, local);
		Vector3 wpB = Vector3::Transform(pB, local);

		// 衝突情報の保存
		Vector3 diff = wpB - wpS;
		float dist = diff.Length();

		if (dist > 0.0001f) {
			info->hitDir = diff / dist;
		}
		else {
			info->hitDir = Vector3::Up; // 重なりすぎている時は真上に逃がす
		}
		info->hitLen = radius - dist;
		info->own = col1->GetOwn();
		info->target = col2->GetOwn();
	}

	return hit;
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
	lB = bSize.x;
	l = abs(nbY.Dot(interval));
	if (lA + lB < l) return false;
	if ((lA + lB) - l < minOverLap)
	{
		minOverLap = (lA + lB) - l;
		minAxis = nbY;
	}

	// Z軸
	lA = LengthSegmentOnSeparateAxis(nbZ, aX, aY, aZ);
	lB = bSize.x;
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

bool CheckAABB(const AABB& aabb1, const AABB& aabb2)
{
	if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) return false;
	if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) return false;
	if (aabb1.min.z > aabb2.max.z || aabb2.min.z > aabb1.max.z) return false;

	return true;
}
