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
	, m_testCamera{ Screen::WIDTH, Screen::HEIGHT }
	, m_objects{}
{
}

TestScene::~TestScene()
{
}

// 初期化関数
void TestScene::Initialize()
{
	m_testCamera.GetComponent<Transform>()->SetLocalPosition({ 0, 5, 15 });
	m_testCamera.GetComponent<Transform>()->SetLocalEulerAngle({ -PI_F / 12, 0, 0 });

	int ballCount = 100;
	for (int i = 0; i < ballCount; i++)
	{
		SimpleMath::Vector3 pos = { Random::GetFloat(-2.0f, 2.0f), Random::GetFloat(5.0f, 9.0f), Random::GetFloat(-2.0f, 2.0f) };

		m_objects.push_back(ObjectManager::Instance().Generate<SphereObject>(SimpleMath::Vector3{ pos }, 0.5));
	}

	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -10, -2, -10 }, SimpleMath::Vector3{ 10, -1.5, 10 }));
//	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -10, 10, -10 }, SimpleMath::Vector3{ 10, 10.5, 10 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -10.5, -2, -10 }, SimpleMath::Vector3{ -10, 10, 10 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{  10, -2, -10 }, SimpleMath::Vector3{  10.5, 10, 10 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -10, -2, 10 }, SimpleMath::Vector3{ 10, 10, 10.5 }));
	m_objects.push_back(ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -10, -2, -10.5 }, SimpleMath::Vector3{ 10, 10, -10 }));

	auto* obj = ObjectManager::Instance().Generate<WallObject>(SimpleMath::Vector3{ -2, -2, -2 }, SimpleMath::Vector3{ 2, 2, 2 });
	obj->GetComponent<Transform>()->SetLocalEulerAngle({ PI_F / 4, PI_F / 4, PI_F / 4 });

	m_objects.push_back(obj);
	CollideManager::Instance().SetCollideActive(5, 5, false);
}

// 更新関数 
void TestScene::Update(float elapsedTime)
{
	CameraMove(elapsedTime);

	m_testCamera.Update();

//	m_testCube->GetComponent<Transform>()->AddLocalEulerAngle({ 0, elapsedTime, 0 });
//	m_testCube2->GetComponent<Transform>()->AddLocalEulerAngle({ 0, 0, elapsedTime / 2 });
//
//	m_testCube3->GetComponent<Transform>()->AddLocalPosition({ 0, -elapsedTime * 2, 0});
//	m_testCube3->GetComponent<Transform>()->AddLocalEulerAngle({ 0, -elapsedTime * 3, 0});
}

// 描画関数
void TestScene::Render()
{
	MyRenderer::SetProjection(m_testCamera.GetProj());
	MyRenderer::SetView(m_testCamera.GetView());
}

// 終了関数
void TestScene::Finalize()
{
}

void TestScene::CameraMove(float elapsedTime)
{
	// カメラのトランスフォームを取得
	Transform* t = m_testCamera.GetComponent<Transform>();

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
