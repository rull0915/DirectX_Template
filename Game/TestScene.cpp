#include "TestScene.h"
#include "Game.h"

#include "GameLib/GameObject/Managers/ObjectManager.h"

#include "TestObjects/TestSphere.h"
#include "TestObjects/WallObject.h"

#include "GameLib/GameObject/Managers/3DManagers/Collider/CollideManager.h"

#include "GameLib/Random.h"

#include "GameLib/Input/MouseInput.h"
#include "GameLib/Input/KeyInput.h"

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
	m_testCamera = ObjectManager::Instance().Generate<GameObject>();

	m_testCamera->GetComponent<Transform>()->SetLocalPosition({ 0, 0, 5 });
//	m_testCamera->GetComponent<Transform>()->SetLocalEulerAngle({ -PI_F / 12, 0, 0 });

	m_testCamera->AddComponent<CameraComponent>(Screen::WIDTH, Screen::HEIGHT)->SetMain();

	{
		auto p1 = ObjectManager::Instance().Generate<GameObject>();
		auto t1 = p1->GetComponent<Transform>();
		p1->AddComponent<BoxCollider2D>();

		t1->SetLocalScale({ 100, 1, 1 });

		auto p2 = ObjectManager::Instance().Generate<GameObject>();
		auto t2 = p2->GetComponent<Transform>();
		p2->AddComponent<BoxCollider2D>();

		t2->SetLocalPosition({ 50, 5, 1 });
		t2->SetLocalScale({ 1, 50, 1 });

		auto p3 = ObjectManager::Instance().Generate<GameObject>();
		auto t3 = p3->GetComponent<Transform>();
		p3->AddComponent<BoxCollider2D>();

		t3->SetLocalPosition({ -50, 5, 1 });
		t3->SetLocalScale({ 1, 50, 1 });

	}

	for (int i = 0; i < 0; i++)
	{
		auto p = ObjectManager::Instance().Generate<GameObject>();

		switch (Random::Get(1, 2))
		{
		case 0:
			p->AddComponent<CircleCollider2D>();
			break;
		case 1:
			p->AddComponent<BoxCollider2D>();
			break;
		case 2:
			p->AddComponent<CapsuleCollider2D>(2.0f, 0.3f);
			break;
		default:
			break;
		} 

		p->AddComponent<RigidBody2D>()->SetFriction(0.1f);
//		p->GetComponent<RigidBody2D>()->SetUseGravity(false);

		SimpleMath::Vector2 pos = { Random::GetFloat(-8.0, 8.0), Random::GetFloat(0.0f, 9.0f) };
		auto t = p->GetComponent<Transform>();

		t->SetLocalPosition({pos.x, pos.y, 1});
		t->SetLocalEulerAngle({ 0, 0, Random::GetFloat(-10.0f, 10.0f) });

		m_objects.push_back(p);
	}

	int ballCount = 500;
	for (int i = 0; i < ballCount; i++)
	{
		SimpleMath::Vector3 pos = { Random::GetFloat(-10.0f, 10.0f), Random::GetFloat(3.0f, 15.0f), Random::GetFloat(-10.0f, 10.0f) };

		m_objects.push_back(ObjectManager::Instance().Generate<SphereObject>(SimpleMath::Vector3{ pos }, 0.5));
	}

	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20, -2, -20 }, SimpleMath::Vector3{ 20, -1.5, 20 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20.5, -2, -20 }, SimpleMath::Vector3{ -20, 20, 20 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{  20, -2, -20 }, SimpleMath::Vector3{  20.5, 20, 20 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20, -2, 20 }, SimpleMath::Vector3{ 20, 20, 20.5 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20, -2, -20.5 }, SimpleMath::Vector3{ 20, 20, -20 }));

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

		SimpleMath::Vector4 rayEye = SimpleMath::Vector4::Transform(rayClip, m_testCamera->GetComponent<CameraComponent>()->GetInverseProj());

		rayEye.z = -1;
		rayEye.w = 0;

		SimpleMath::Vector4 rayWorld = SimpleMath::Vector4::Transform(rayEye, m_testCamera->GetComponent<CameraComponent>()->GetInverseView());

		// 正規化
		rayWorld.Normalize();

		// コライダー情報を更新
		SimpleMath::Vector3 start = pos;
		SimpleMath::Vector3 end = start + rayWorld ;

		auto p = ObjectManager::Instance().Generate<SphereObject>(pos, 0.5f);
		p->GetComponent<RigidBody>()->AddForce((end - start) * p->GetComponent<RigidBody>()->GetMass() * 2000);
	}

	if (KeyInput::GetKeyDown(DirectX::Keyboard::Keys::Enter))
	{
				auto p = ObjectManager::Instance().Generate<GameObject>();

		switch (Random::Get(1, 2))
		{
		case 0:
			p->AddComponent<CircleCollider2D>();
			break;
		case 1:
			p->AddComponent<BoxCollider2D>();
			break;
		case 2:
			p->AddComponent<CapsuleCollider2D>(2.0f, 0.5f);
			break;
		default:
			break;
		} 

		p->AddComponent<RigidBody2D>()->SetFriction(0.1f);
//		p->GetComponent<RigidBody2D>()->SetUseGravity(false);

		SimpleMath::Vector2 pos = { Random::GetFloat(-8.0f, 8.0f), Random::GetFloat(3.0f, 9.0f) };
		auto t = p->GetComponent<Transform>();

		t->SetLocalPosition({pos.x, pos.y, 1});
		t->SetLocalEulerAngle({ 0, 0, Random::GetFloat(-10.0f, 10.0f) });

		m_objects.push_back(p);
//		m_testCamera->GetComponent<CameraComponent>()->ChangeType();
	}
}

// 描画関数
void TestScene::Render()
{
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
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::W)) t->AddLocalPosition(t->GetForward() * 5 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::A)) t->AddLocalPosition(t->GetRight() * -5 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::S)) t->AddLocalPosition(t->GetForward() * -5 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::D)) t->AddLocalPosition(t->GetRight() * 5 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Space)) t->AddLocalPosition(t->GetUp () * 5 * elapsedTime);
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::LeftShift)) t->AddLocalPosition(t->GetUp() * -5 * elapsedTime);

	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Left)) t->AddLocalEulerAngle({0, elapsedTime, 0});
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Right)) t->AddLocalEulerAngle({ 0,-elapsedTime, 0 });
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Up)) t->AddLocalEulerAngle({ elapsedTime, 0, 0 });
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::Down)) t->AddLocalEulerAngle({ -elapsedTime, 0, 0 });

	//Transform* t = m_objects[0]->GetComponent<Transform>();

	//if (KeyInput::GetKey(DirectX::Keyboard::Keys::W)) t->AddLocalPosition(DirectX::SimpleMath::Vector3{ 0, 1, 0 } * 5 * elapsedTime);
	//if (KeyInput::GetKey(DirectX::Keyboard::Keys::A)) t->AddLocalPosition(DirectX::SimpleMath::Vector3{ 1, 0, 0 } * -5 * elapsedTime);
	//if (KeyInput::GetKey(DirectX::Keyboard::Keys::S)) t->AddLocalPosition(DirectX::SimpleMath::Vector3{ 0, 1, 0 } * -5 * elapsedTime);
	//if (KeyInput::GetKey(DirectX::Keyboard::Keys::D)) t->AddLocalPosition(DirectX::SimpleMath::Vector3{ 1, 0, 0 } * 5 * elapsedTime);
}
