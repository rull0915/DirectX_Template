//====================================================//
// ファイル名  : BaseRenderer.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/05/02
//
// 概要       : レンダラーコンポーネントの基底クラス  
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "BaseRenderer.h"
#include "GameLib/GameObject/GameObject.h"

//====================================================//
// 関数の実体宣言
//====================================================//


BaseRenderer::BaseRenderer(GameObject* own, int ID, bool isMain)
    : BaseComponent(own, ID, isMain)
    , m_pTransform{ own->GetComponent<Transform>() }
{
};