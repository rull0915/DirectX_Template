//====================================================//
// ファイル名  : BaseCollider.cpp
// 概要        :  コライダーの基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "BaseCollider.h"

#include "../Managers/CollideManager.h"

#include "../../GameObject.h"
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
{
	// 自身をマネージャーに登録
	CollideManager::Instance().AddCollide(this);
}

BaseCollider::~BaseCollider()
{
	int a = 0;

	// 自身をマネージャーから削除
//	CollideManager::Instance().RemoveCollide(this);
}

void AABB::DebugDraw(int color) const
{
	MyRenderer::Draw3DBox(min, max, color, false);
}
