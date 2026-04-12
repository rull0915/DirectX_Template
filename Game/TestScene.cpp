#include "TestScene.h"
#include "Game.h"

#include "GameLib/GameObject/ObjectManager.h"

#include "TestObjects/TestSphere.h"
#include "TestObjects/WallObject.h"

#include "GameLib/GameObject/Components/Managers/CollideManager.h"

using namespace DirectX;

// コンストラクタ
TestScene::TestScene(Game* pGame)
	: Scene(pGame->GetSceneManager())
	, m_pGame{ pGame }
	, m_testCamera{}
	, m_objects{}
{
}

TestScene::~TestScene()
{
}

// 初期化関数
void TestScene::Initialize()
{
	m_testCamera = ObjectManager::Instance().Generate<Camera>(Screen::WIDTH, Screen::HEIGHT);

	m_testCamera->GetComponent<Transform>()->SetLocalPosition({ 0, 8, 15 });
	m_testCamera->GetComponent<Transform>()->SetLocalEulerAngle({ -PI_F / 12, 0, 0 });

	int ballCount = 200;
	for (int i = 0; i < ballCount; i++)
	{
		SimpleMath::Vector3 pos = { Random::GetFloat(-2.0f, 2.0f), Random::GetFloat(5.0f, 9.0f), Random::GetFloat(-2.0f, 2.0f) };

		m_objects.push_back(ObjectManager::Instance().Generate<SphereObject>(SimpleMath::Vector3{ pos }, 0.5));
	}

	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -8, -2, -8 }, SimpleMath::Vector3{ 8, -1.5, 8 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -8.5, -2, -8 }, SimpleMath::Vector3{ -8, 8, 8 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{  8, -2, -8 }, SimpleMath::Vector3{  8.5, 8, 8 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -8, -2, 8 }, SimpleMath::Vector3{ 8, 8, 8.5 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -8, -2, -8.5 }, SimpleMath::Vector3{ 8, 8, -8 }));

	auto* obj = ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -2, -2, -2 }, SimpleMath::Vector3{ 2, 2, 2 });
	obj->GetComponent<Transform>()->SetLocalEulerAngle({ PI_F / 4, PI_F / 4, PI_F / 4 });

	m_objects.push_back(obj);
	CollideManager::Instance().SetCollideActive(5, 5, false);
}

// 更新関数 
void TestScene::Update(float elapsedTime)
{
	CameraMove(elapsedTime);

	if (MouseInput::GetMouseDown(MOUSE_LEFT))
	{
		DirectX::SimpleMath::Vector3 pos = m_testCamera->GetComponent<Transform>()->GetWorldPosition();

		// マウスの位置を取得
		DirectX::SimpleMath::Vector2 mousePos = MouseInput::GetMousePoint();

		float x = (2.0f * mousePos.x) / Screen::WIDTH - 1.0f;
		float y = 1.0f - (2.0f * mousePos.y) / Screen::HEIGHT;

		// 逆行列で変換しワールド座標系に
		DirectX::SimpleMath::Vector4 rayClip = { x, y, 1, 1 };

		SimpleMath::Vector4 rayEye = SimpleMath::Vector4::Transform(rayClip, m_testCamera->GetInverseProj());

		rayEye.z = -1;
		rayEye.w = 0;

		SimpleMath::Vector4 rayWorld = SimpleMath::Vector4::Transform(rayEye, m_testCamera->GetInverseView());

		// 正規化
		rayWorld.Normalize();

		// コライダー情報を更新
		SimpleMath::Vector3 start = pos;
		SimpleMath::Vector3 end = start + rayWorld ;

		auto p = ObjectManager::Instance().Generate<SphereObject>(pos, 0.5f);
		p->GetComponent<RigidBody>()->AddForce((end - start) * p->GetComponent<RigidBody>()->GetMass() * 5000);
	}

	MyRenderer::SetProjection(m_testCamera->GetProj());
	MyRenderer::SetView(m_testCamera->GetView());
}

// 描画関数
void TestScene::Render()
{
	MyRenderer::SetProjection(m_testCamera->GetProj());
	MyRenderer::SetView(m_testCamera->GetView());
}

// 終了関数
void TestScene::Finalize()
{
}

void TestScene::CameraMove(float elapsedTime)
{
	// カメラのトランスフォームを取得
	Transform* t = m_testCamera->GetComponent<Transform>();

	// 移動
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::W)) t->AddLocalPosition(t->GetForward() * 2 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::A)) t->AddLocalPosition(t->GetRight() * -2 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::S)) t->AddLocalPosition(t->GetForward() * -2 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::D)) t->AddLocalPosition(t->GetRight() * 2 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Space)) t->AddLocalPosition(t->GetUp () * 2 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::LeftShift)) t->AddLocalPosition(t->GetUp() * -2 * elapsedTime);

	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Left)) t->AddLocalEulerAngle({0, elapsedTime, 0});
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Right)) t->AddLocalEulerAngle({ 0,-elapsedTime, 0 });
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Up)) t->AddLocalEulerAngle({ elapsedTime, 0, 0 });
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Down)) t->AddLocalEulerAngle({ -elapsedTime, 0, 0 });
}
