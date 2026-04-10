#include "pch.h"
#include "TestSphere.h"

SphereObject::SphereObject(DirectX::SimpleMath::Vector3 pos, float radius)
{
	// コンポーネントの追加
	AddComponent<RigidBody>()->SetRestitution(0.8f);
	AddComponent<RigidBody>()->SetMass(1.0f);
	AddComponent<BoxCollider>();

	auto* pTrans = GetComponent<Transform>();
	pTrans->SetLocalPosition( pos );
	pTrans->SetLocalScale({ radius * 2, radius * 2, radius * 2 });
}

void SphereObject::Initialize()
{
}

void SphereObject::Update(float elapsedTime)
{
}

void SphereObject::Render()
{
}

void SphereObject::Finalize()
{
}

void SphereObject::OnCollision(BaseCollider* col)
{
}
