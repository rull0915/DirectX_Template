#include "FadeTransition.h"

#include "GameLib/MyRenderer.h"

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
	// Œo‰ßŽžŠÔ‚Ì‰ÁŽZ
	m_elapsedTime += elapsedTime;

	if (m_elapsedTime >= m_transSec) m_elapsedTime = m_transSec;

	// ‘JˆÚŽžŠÔ‚ð’´‚¦‚½‚©‚Ç‚¤‚©
	return (m_elapsedTime >= m_transSec);
}

void FadeTransition::Render()
{
	// “§–¾“x‚ÌŒvŽZ
	float alpha = 0;

	// •`‰æ

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
