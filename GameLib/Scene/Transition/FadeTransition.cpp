//====================================================//
// ファイル名  : FadeTransition.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/01
//
// 概要        :  フェードイン/アウト
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "FadeTransition.h"

#include "GameLib/MyRenderer.h"
#include "Game/Screen.h"

//====================================================//
// 関数の実体宣言
//====================================================//

FadeTransition::FadeTransition(float transTime, Transition::Mode mode, int fadeColor)
	: SceneTransition(transTime, mode)
	, m_fadeColor{ fadeColor }
{
}

void FadeTransition::Initialize()
{
}

bool FadeTransition::Update(float elapsedTime)
{
	// 経過時間の加算
	m_elapsedTime += elapsedTime;

	if (m_elapsedTime >= m_transSec) m_elapsedTime = m_transSec;

	// 遷移時間を超えたかどうか
	return (m_elapsedTime >= m_transSec);
}

void FadeTransition::Render()
{
	// 透明度の計算
	float alpha = 0;

	// 描画

	switch (m_mode)
	{
	case Transition::Mode::In:

		alpha = 255 * (m_elapsedTime / m_transSec);

		break;
	case Transition::Mode::Out:

		alpha = 255 - 255 * (m_elapsedTime / m_transSec);

		break;
	default:
		break;
	}

	MyRenderer::SetAlpha(alpha / 255.0f);

	MyRenderer::DrawBox({ 0, 0 }, { Screen::WIDTH, Screen::HEIGHT }, m_fadeColor);

	MyRenderer::SetAlpha(1.0f);
}
