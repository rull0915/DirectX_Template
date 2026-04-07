//====================================================//
// ファイル名  : BaseCollider.cpp
// 概要        :  コライダーの基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "BaseCollider.h"

#include "CollideManager.h"

#include "../../GameObject.h"
#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
BaseCollider::BaseCollider(GameObject* own, ColliderType type, int ID, bool isMain)
	: BaseComponent(own, ID, isMain)
	, m_type{ type }
	, m_pTransform{ own->GetComponent<Transform>() }
	, m_boundingBox{ {0, 0, 0}, {0, 0, 0} }
	, m_isTrigger{ false }
{
	// 自身をマネージャーに登録
	CollideManager::Instance().AddCollide(this);
}

BaseCollider::~BaseCollider()
{
	// 自身をマネージャーから削除
	CollideManager::Instance().RemoveCollide(this);
}

void AABB::DebugDraw(int color) const
{
	MyRenderer::Draw3DBox(min, max, color, false);
}
