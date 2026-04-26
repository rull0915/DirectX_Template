#include "CubeObject.h"

#include "GameLib/MyRenderer.h"

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
	MyRenderer::SetWorld(GetComponent<Transform>()->GetWorldMatrix());

//	MyRenderer::Draw3DBox({ -0.5, -0.5, -0.5 }, { 0.5, 0.5, 0.5 }, 0xFF0000, !KeyInput::GetKey(DirectX::Keyboard::Keys::F1));

	MyRenderer::SetWorld(DirectX::SimpleMath::Matrix::Identity);
}

void CubeObject::Finalize()
{
}

void CubeObject::OnCollisionEnter(BaseCollider* col)
{
	col;

	m_debugColor = 0xFF4444;
}
