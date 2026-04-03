#include "RayCast.h"

#include "GameLib/MyRenderer.h"

using namespace DirectX;

RayCast::RayCast()
{
}

RayCast::~RayCast()
{
}

void RayCast::Initialize()
{
//	AddComponent<LineCollider>(SimpleMath::Vector3{0, 0, 0}, SimpleMath::Vector3{0, 0, 0});
}

void RayCast::Update(Camera& camera)
{
	// マウスの位置を取得
	DirectX::SimpleMath::Vector2 mousePos = MouseInput::GetMousePoint();

	float x = (2.0f * mousePos.x) / Screen::WIDTH - 1.0f;
	float y = 1.0f - (2.0f * mousePos.y) / Screen::HEIGHT;

	// 逆行列で変換しワールド座標系に
	DirectX::SimpleMath::Vector4 rayClip = { x, y, 1, 1 };

	SimpleMath::Vector4 rayEye = SimpleMath::Vector4::Transform(rayClip, camera.GetInverseProj());

	rayEye.z = -1;
	rayEye.w = 0;

	SimpleMath::Vector4 rayWorld = SimpleMath::Vector4::Transform(rayEye, camera.GetInverseView());

	// 正規化
	rayWorld.Normalize();

	// コライダー情報を更新
	SimpleMath::Vector3 start = camera.GetPos();
	SimpleMath::Vector3 end = start + rayWorld * 1000;

	//if (!GetCollider().empty())
	//{
	//	auto col = dynamic_cast<LineCollider*>(GetCollider().at(0).get());

	//	col->SetStart(start);
	//	col->SetEnd(end);
	//}

	m_pGameObject = nullptr;
}

void RayCast::DebugDraw()
{
	//auto col = dynamic_cast<LineCollider*>(GetCollider().at(0).get());
	//auto start = col->GetStart();
	//start.z -= 0.1f;
	//MyRenderer::DrawLine(start, col->GetEnd(), 0xFF0000);
}

void RayCast::OnCollision(BaseCollider* col)
{
//	m_pGameObject = col->GetOwn();
}
