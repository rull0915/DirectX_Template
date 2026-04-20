#include "pch.h"
#include "TestSphere.h"

#include "GameLib/Random.h"

SphereObject::SphereObject(DirectX::SimpleMath::Vector3 pos, float radius)
{
	// コンポーネントの追加
	auto rid = AddComponent<RigidBody>();

	rid->SetMass(Random::GetFloat(0.5f, 5.0f));
//	rid->SetRestitution(0.5f);
//	AddComponent<RigidBody>()->SetUseGravity(false);

//	AddComponent<SphereCollider>();

	int rand = Random::Get(0, 2);
	switch (rand)
	{
	case 0: 
		AddComponent<BoxCollider>();
		break;
	case 1:
		AddComponent<SphereCollider>();
		break;
	case 2:
		AddComponent<CapsuleCollider>(Random::GetFloat(1.0f, 3.0f), Random::GetFloat(0.1f, 0.75f));
		break;
	default:
		break;
	}

	m_pTrans = GetComponent<Transform>();

	m_pTrans->SetLocalPosition( pos );
	m_pTrans->SetLocalEulerAngle({ Random::GetFloat(0.0f, PI_F * 2), Random::GetFloat(0.0f, PI_F * 2), Random::GetFloat(0.0f, PI_F * 2) });
	m_pTrans->SetLocalScale({ Random::GetFloat(0.3f, 2.0f), Random::GetFloat(0.3f, 2.0f), Random::GetFloat(0.3f, 2.0f) });
}

void SphereObject::Initialize()
{
}

void SphereObject::Update(float elapsedTime)
{
	elapsedTime;

	m_pTrans->UpdateCache();
}

void SphereObject::Render()
{
}

void SphereObject::Finalize()
{
}

void SphereObject::OnCollision(BaseCollider* col)
{
	col;
}
