#include "TestScene.h"
#include "Game.h"

// コンストラクタ
TestScene::TestScene(Game* pGame)
	: Scene(pGame->GetSceneManager())
	, m_pGame{ pGame }
	, m_testCamera{ Screen::WIDTH, Screen::HEIGHT }
	, m_testCube{}
	, m_testCube2{}
	, m_testCube3{}
{
}

TestScene::~TestScene()
{
}

// 初期化関数
void TestScene::Initialize()
{
	m_testCamera.GetComponent<Transform>()->SetLocalPosition({ 0, 0, 10 });

	m_testCube2.GetComponent<Transform>()->SetParent(m_testCube.GetComponent<Transform>());
	m_testCube2.GetComponent<Transform>()->SetLocalPosition({ 1, 2, 0 });
	m_testCube2.GetComponent<Transform>()->SetLocalScale({ 0.5, 0.5, 0.5 });

//	m_testCube3.GetComponent<Transform>()->SetParent(m_testCube2.GetComponent<Transform>());
	m_testCube3.GetComponent<Transform>()->SetLocalPosition({ 0, 10, 0 });
//	m_testCube3.GetComponent<Transform>()->SetLocalScale({ 0.5, 0.5, 0.5 });

	//	AddComponent<CapsuleCollider>(3.0f, 1.0f);
//	AddComponent<BoxCollider>();
	m_testCube3.AddComponent <BoxCollider>();
//	m_testCube.AddComponent<CapsuleCollider>(4.0f, 0.2f);
	m_testCube.AddComponent<BoxCollider>();

	m_testCube.AddComponent<RigidBody>();
	m_testCube3.AddComponent<RigidBody>();
//	m_testCube2.AddComponent<SphereCollider>();

//	m_testCube.GetComponent<Transform>()->SetLocalScale({10, 1, 10});

//	m_testCube.GetComponent<BoxCollider>()->SetLocalSize({1, 1, 1});
	m_testCube.GetComponent<Transform>()->SetLocalEulerAngle({0.5f, 0, 0.7});
	m_testCube3.GetComponent<Transform>()->SetLocalEulerAngle({0, 0, 1});
//	m_testCube.GetComponent<Transform>()->SetLocalScale({5, 1, 5});
}

// 更新関数 
void TestScene::Update(float elapsedTime)
{
	CameraMove(elapsedTime);

	m_testCamera.Update();

	m_testCube.GetComponent<Transform>()->AddLocalEulerAngle({ 0, elapsedTime * 2, 0 });
	m_testCube2.GetComponent<Transform>()->AddLocalEulerAngle({ 0, 0, elapsedTime / 2 });

	m_testCube3.GetComponent<Transform>()->AddLocalPosition({ 0, -elapsedTime, 0});
	m_testCube3.GetComponent<Transform>()->AddLocalEulerAngle({ 0, -elapsedTime, 0});

	m_testCube.Update();
	m_testCube2.Update();
	m_testCube3.Update();
}

// 描画関数
void TestScene::Render()
{
	MyRenderer::SetProjection(m_testCamera.GetProj());
	MyRenderer::SetView(m_testCamera.GetView());

	m_testCube.Render();
	m_testCube2.Render();
	m_testCube3.Render();
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
