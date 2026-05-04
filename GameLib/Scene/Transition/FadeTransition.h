#pragma once
#include "SceneTransition.h"
#include "GameLib/Common/Renderer/Renderer.h"

class FadeTransition : public SceneTransition
{
private:
	int m_fadeColor;

public:
	FadeTransition(float transTime, Transition::Mode mode, int fadeColor = 0x000000);

	void Initialize() override;

	bool Update(float elapsedTime) override;

	void Render(Renderer& renderer) override;
};

