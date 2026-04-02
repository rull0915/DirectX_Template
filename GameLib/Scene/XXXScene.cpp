#include "XXXScene.h"
#include "../Game.h"

// コンストラクタ
XXXScene::XXXScene(Game* pGame)
	: Scene(pGame->GetSceneManager())
	, m_pGame{ pGame }
{
}

XXXScene::~XXXScene()
{
}

// 初期化関数
void XXXScene::Initialize()
{
}

// 更新関数 
void XXXScene::Update(float elapsedTime)
{
	// スペースキーでタイトルへ
	if (m_pGame->GetKeyTrigger() & PAD_INPUT_10)
	{
		ChangeScene("");
	}
}

// 描画関数
void XXXScene::Render()
{
}

// 終了関数
void XXXScene::Finalize()
{
}