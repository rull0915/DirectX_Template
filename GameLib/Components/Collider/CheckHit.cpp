#include "CheckHit.h"

bool CheckHit(BaseCollider* col1, BaseCollider* col2)
{
	switch (col1->GetType())
	{
		// 1つ目が球の場合
	case ColliderType::Sphere:

		switch (col2->GetType())
		{
			// 2つ目が球の場合
		case ColliderType::Sphere:
			return CheckHit(static_cast<SphereCollider*>(col1), static_cast<SphereCollider*>(col2));
			// 2つ目が線の場合
		case ColliderType::Line:
			return CheckHit(static_cast<SphereCollider*>(col1), static_cast<LineCollider*>(col2));
			// 2つ目がAABBの場合
		case ColliderType::AABB:
			return CheckHit(static_cast<SphereCollider*>(col1), static_cast<AABBCollider*>(col2));
		case ColliderType::Capsule:
			break;
		case ColliderType::Box:
			break;
		default:
			break;
		}

		break;

	case ColliderType::Line:

		switch (col2->GetType())
		{
		case ColliderType::Sphere:
			return CheckHit(static_cast<SphereCollider*>(col2), static_cast<LineCollider*>(col1));
		case ColliderType::Line:
			break;
		case ColliderType::AABB:
			return CheckHit(static_cast<LineCollider*>(col1), static_cast<AABBCollider*>(col2));
		case ColliderType::Capsule:
			break;
		case ColliderType::Box:
			break;
		default:
			break;
		}

		break;

	case ColliderType::AABB:

		switch (col2->GetType())
		{
		case ColliderType::Sphere:
			return CheckHit(static_cast<SphereCollider*>(col2), static_cast<AABBCollider*>(col1));
		case ColliderType::Line:
			return CheckHit(static_cast<LineCollider*>(col2), static_cast<AABBCollider*>(col1));
		case ColliderType::AABB:
			return CheckHit(static_cast<AABBCollider*>(col1), static_cast<AABBCollider*>(col2));
		case ColliderType::Capsule:
			break;
		case ColliderType::Box:
			break;
		default:
			break;
		}

		break;

	case ColliderType::Capsule:

		switch (col2->GetType())
		{
		case ColliderType::Sphere:
			break;
		case ColliderType::Line:
			break;
		case ColliderType::AABB:
			break;
		case ColliderType::Capsule:
			break;
		case ColliderType::Box:
			break;
		default:
			break;
		}

		break;

	case ColliderType::Box:

		switch (col2->GetType())
		{
		case ColliderType::Sphere:
			break;
		case ColliderType::Line:
			break;
		case ColliderType::AABB:
			break;
		case ColliderType::Capsule:
			break;
		case ColliderType::Box:
			break;
		default:
			break;
		}

		break;

	default:
		break;
	}

	return false;
}

bool CheckHit(SphereCollider* col1, SphereCollider* col2)
{
	// 中心座標の距離を算出
	float length = (col1->GetCenterPos() - col2->GetCenterPos()).LengthSquared();

	// 半径の合計以下かどうかを返す
	return length <= (col1->GetRadius() + col2->GetRadius()) * (col1->GetRadius() + col2->GetRadius());
}

bool CheckHit(SphereCollider* col1, LineCollider* col2)
{
	auto start = col2->GetStart();
	auto end = col2->GetEnd();
	auto center = col1->GetCenterPos();

	// 線分のベクトルを作成
	DirectX::SimpleMath::Vector3 dir = (end - start);

	// 線分の開始位置から球の中心へのベクトルを作成
	DirectX::SimpleMath::Vector3 toCenter = (center - start);

	// --- 線分から球の中心への最近点を求める ---

	// 最近点への媒介変数の値を求める
	float lenSq = dir.Dot(dir);
	if (lenSq <= 0.000001f)
		return false;

	float t = toCenter.Dot(dir) / lenSq;
	t = MyMath::Clamp(t, 0, 1);

	// 最近点を求める
	DirectX::SimpleMath::Vector3 nearPoint = start + dir * t;

	// --- 最近点と球の中心との距離と半径を比較 ---
	float r = col1->GetRadius();
	return (nearPoint - center).LengthSquared() <= r * r;
}

bool CheckHit(SphereCollider* col1, AABBCollider* col2)
{
	auto center = col1->GetCenterPos();
	auto min = col2->GetMinPos();
	auto max = col2->GetMaxPos();

	// AABB上で球の中心に最も近い点
	DirectX::SimpleMath::Vector3 nearPoint;

	nearPoint.x = MyMath::Clamp(center.x, min.x, max.x);
	nearPoint.y = MyMath::Clamp(center.y, min.y, max.y);
	nearPoint.z = MyMath::Clamp(center.z, min.z, max.z);

	// 最近点から半径の距離が半径以下かどうかを調べる
	float distSq = (nearPoint - center).LengthSquared();

	float r = col1->GetRadius();

	return distSq <= r * r;
}

bool CheckHit(LineCollider* col1, AABBCollider* col2)
{
	DirectX::SimpleMath::Vector3 start = col1->GetStart(), end = col1->GetEnd();
	DirectX::SimpleMath::Vector3 min = col2->GetMinPos(), max = col2->GetMaxPos();

	DirectX::SimpleMath::Vector3 dir = end - start;

	// --- 媒介変数がとりうる範囲を各成分で求める ---
	MyMath::Range tXRange = MyMath::Range(-FLT_MAX, FLT_MAX), tYRange = MyMath::Range(-FLT_MAX, FLT_MAX), tZRange = MyMath::Range(-FLT_MAX, FLT_MAX);

	// x成分
	if (abs(dir.x) < 0.00001f)
	{
		if (start.x < min.x || start.x > max.x) return false;
	}
	else
	{
		float tXMin = (min.x - start.x) / dir.x;
		float tXMax = (max.x - start.x) / dir.x;

		tXRange = MyMath::Range(std::min(tXMin, tXMax), std::max(tXMin, tXMax));
	}

	// y成分
	if (abs(dir.y) < 0.00001f)
	{
		if (start.y < min.y || start.y > max.y) return false;
	}
	else
	{
		float tYMin = (min.y - start.y) / dir.y;
		float tYMax = (max.y - start.y) / dir.y;

		tYRange = MyMath::Range(std::min(tYMin, tYMax), std::max(tYMin, tYMax));
	}

	// z成分
	if (abs(dir.z) < 0.00001f)
	{
		if (start.z < min.z || start.z > max.z) return false;
	}
	else
	{
		float tZMin = (min.z - start.z) / dir.z;
		float tZMax = (max.z - start.z) / dir.z;

		tZRange = MyMath::Range(std::min(tZMin, tZMax), std::max(tZMin, tZMax));
	}

	// 共通部分が存在するかをチェック
	MyMath::Range tRange = MyMath::CommonPart(tXRange, MyMath::CommonPart(tYRange, tZRange));

	MyMath::Range result = MyMath::CommonPart(tRange, MyMath::Range(0, 1));

	return !result.IsNull();
}

bool CheckHit(AABBCollider* col1, AABBCollider* col2)
{
	DirectX::SimpleMath::Vector3 min1 = col1->GetMinPos(), max1 = col1->GetMaxPos();
	DirectX::SimpleMath::Vector3 min2 = col2->GetMinPos(), max2 = col2->GetMaxPos();

	bool x = min1.x <= max2.x && min2.x <= max1.x;
	bool y = min1.y <= max2.y && min2.y <= max1.y;
	bool z = min1.z <= max2.z && min2.z <= max1.z;

	return x && y && z;
}

bool CheckAABB(const AABB& aabb1, const AABB& aabb2)
{
	if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) return false;
	if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) return false;
	if (aabb1.min.z > aabb2.max.z || aabb2.min.z > aabb1.max.z) return false;

	return true;
}
