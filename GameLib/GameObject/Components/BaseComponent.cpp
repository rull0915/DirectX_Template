//====================================================//
// ファイル名  : BaseComponent.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要       : コンポーネントの基底クラス  
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "BaseComponent.h"

#include "GameLib/GameObject/GameObject.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
BaseComponent::BaseComponent(GameObject* own, int id, bool isMain)
	: m_own{ own }
	, m_isActive{ true }
	, m_myID{ id }
	, m_isMain{ isMain }
{
}

/// <summary>
/// デストラクタ
/// </summary>
BaseComponent::~BaseComponent()
{
}
