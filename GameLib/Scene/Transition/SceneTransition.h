#pragma once

#include "GameLib/Common/Renderer/Renderer.h"

// 遷移モード
namespace Transition
{
	enum class Mode
	{
		In,
		Out
	};
}

// シーン遷移基底クラス
class SceneTransition
{
	// ----- 変数宣言 ----- //
protected:
	// 遷移する時間
	float m_transSec;

	// 遷移のモード
	Transition::Mode m_mode;

	// 経過時間
	float m_elapsedTime;

	// ----- 関数宣言 ----- //
public:
	SceneTransition(float transSec, Transition::Mode mode);
	virtual ~SceneTransition() = default;

	// 純粋仮想関数
	virtual void Initialize() = 0;
	
	// 遷移終了時にtrue
	virtual bool Update(float elapsedTime) = 0;

	virtual void Render(Renderer& renderer) = 0;
};

