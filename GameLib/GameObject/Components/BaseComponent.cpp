//====================================================//
// ファイル名  : BaseComponent.cpp
// 概要       : コンポーネントの基底クラス  
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "BaseComponent.h"

#include "../GameObject.h"

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
