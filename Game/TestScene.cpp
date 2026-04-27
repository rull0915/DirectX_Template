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
	, m_material{0.2f, 0.1f, 0.6f, CombineMode::Minimum, CombineMode::Average}
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

	m_testCamera->AddComponent<CameraComponent>(Screen::WIDTH, Screen::HEIGHT)->SetMain();

	int ballCount = 5;

	for (int i = 0; i < ballCount; i++)
	{
		SimpleMath::Vector3 pos = { -20, 0, (i - (float)ballCount / 2) * 3 };
//		SimpleMath::Vector3 pos = { (i - (float)ballCount / 2) * 3, 10, 0 };

		auto cube = ObjectManager::Instance().Generate<BoxTemplate>();
		cube->SetPosition(pos);

		auto rid = cube->AddComponent<RigidBody>();

		rid->SetLinearDamping(i);
//		rid->SetUseGravity(false);

		cube->GetComponent<BaseCollider>()->SetPhysicsMaterial(&m_material);

		m_cubes.push_back(cube);
	}

	m_floor = (ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -60, -2, -60 }, SimpleMath::Vector3{ 60, -1.5, 60 }));
	m_floor->GetComponent<Transform>()->SetLocalEulerAngle({ 0, 0, 0 });
	//m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20.5, -2, -20 }, SimpleMath::Vector3{ -20, 20, 20 }));
	//m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{  20, -2, -20 }, SimpleMath::Vector3{  20.5, 20, 20 }));
	//m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20, -2, 20 }, SimpleMath::Vector3{ 20, 20, 20.5 }));
	//m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -20, -2, -20.5 }, SimpleMath::Vector3{ 20, 20, -20 }));

	CollideManager::Instance().SetCollideActive(5, 5, false);
}

// 更新関数 
void TestScene::Update(float elapsedTime)
{
	CameraMove(elapsedTime);

	if (KeyInput::GetKeyDown(DirectX::Keyboard::Keys::Z))
	{
		static DirectX::SimpleMath::Vector3 force = { 1500.0f, 1500.0f, 0.0f };

		for (auto cube : m_cubes)
		{
			auto rid = cube->GetComponent<RigidBody>();
			if (rid)
			{
				rid->AddForce(force);
//				rid->SetUseGravity(true);
			}
		}
	}
	if (KeyInput::GetKey(DirectX::Keyboard::Keys::X))
	{
		m_floor->GetComponent<Transform>()->AddLocalEulerAngle({ 0, 0, (XM_PI / 8) * elapsedTime });
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
