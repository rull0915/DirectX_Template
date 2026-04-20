//====================================================//
// ファイル名  : BaseCollider.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要        :  コライダーの基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "BaseCollider.h"

#include "GameLib/GameObject/3D/Managers/CollideManager.h"

#include "GameLib/GameObject/3D/GameObject.h"
#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
BaseCollider::BaseCollider(GameObject* own, ColliderType type, int ID, bool isMain, DirectX::SimpleMath::Vector3 center)
	: BaseComponent(own, ID, isMain)
	, m_type{ type }
	, m_pTransform{ own->GetComponent<Transform>() }
	, m_boundingBox{ {0, 0, 0}, {0, 0, 0} }
	, m_isTrigger{ false }
	, m_isDirty{ true }
	, m_localCenterPos{ center }
	, m_isChanged{ false }
	, m_layerNum{ -1 }
{
	// 自身をマネージャーに登録
	CollideManager::Instance().AddCollide(this);
}

BaseCollider::~BaseCollider()
{
}

void AABB::DebugDraw(int color) const
{
	MyRenderer::Draw3DBox(min, max, color, false);
}
