#include "RayCast.h"

#include "GameLib/MyRenderer.h"

using namespace DirectX;

RayCast::RayCast()
	: m_pGameObject{ nullptr }
	, m_shortestDist{ FLT_MAX }
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
	// 最短距離をリセット
	m_shortestDist = FLT_MAX;

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
	SimpleMath::Vector3 start = camera.GetComponent<Transform>()->GetWorldPosition();
	SimpleMath::Vector3 end = start + rayWorld * 1000;

	auto col = GetComponent<LineCollider>();

	col->SetStart(start);
	col->SetEnd(end);

	m_pGameObject = nullptr;
}

void RayCast::DebugDraw()
{
}

void RayCast::OnCollision(BaseCollider* col)
{
	// レイの開始地点
	SimpleMath::Vector3 camPos = GetComponent<Transform>()->GetWorldPosition();
	// 今回当たったオブジェクトの位置
	SimpleMath::Vector3 hitPos = col->GetOwn()->GetComponent<Transform>()->GetWorldPosition();

	float dist = SimpleMath::Vector3::DistanceSquared(camPos, hitPos);

	// 初めての衝突、または今までの最短距離より近ければ更新
	if (m_pGameObject == nullptr || dist < m_shortestDist) {
		m_pGameObject = col->GetOwn();
		m_shortestDist = dist;
	}
}
