#include "pch.h"
#include "WallObject.h"

WallObject::WallObject(DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end)
{
	// コンポーネントの追加
	auto* rid = AddComponent<RigidBody>();
	rid->SetFriction(0.1f);
//	rid->SetRestitution(0.5f);
	rid->SetStatic(true);

	AddComponent<BoxCollider>()->SetLayer(5);

	auto* pTrans = GetComponent<Transform>();

	DirectX::SimpleMath::Vector3 center = (start + end) / 2;
	DirectX::SimpleMath::Vector3 size = { abs(end.x - start.x), abs(end.y - start.y), abs(end.z - start.z) };
	pTrans->SetLocalPosition(center);
	pTrans->SetLocalScale(size);
}

void WallObject::Initialize()
{
}

void WallObject::Update(float elapsedTime)
{
	elapsedTime;
}

void WallObject::Render()
{
}

void WallObject::Finalize()
{
}

void WallObject::OnCollision(BaseCollider* col)
{
	col;
}
