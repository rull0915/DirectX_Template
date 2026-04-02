#include "SceneTransition.h"

SceneTransition::SceneTransition(float transSec, Transition::Mode mode)
	: m_transSec{ transSec }
	, m_mode{ mode }
	, m_elapsedTime{ 0 }
{
}
