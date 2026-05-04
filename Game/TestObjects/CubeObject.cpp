#include "CubeObject.h"

CubeObject::CubeObject()
{
}

CubeObject::~CubeObject()
{
}

void CubeObject::Initialize()
{
	// コンポーネントの追加
//	AddComponent<RigidBody>();
//	AddComponent<BoxCollider>();
}

void CubeObject::Update(float elapsedTime)
{
	elapsedTime;

	m_debugColor = 0x00FF00;
}

void CubeObject::Render()
{
}

void CubeObject::Finalize()
{
}

void CubeObject::OnCollisionEnter(BaseCollider* col)
{
	col;

	m_debugColor = 0xFF4444;
}
