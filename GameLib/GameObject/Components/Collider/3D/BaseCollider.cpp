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

#include "GameLib/GameObject/Managers/3DManagers/Collider/CollideManager.h"

#include "GameLib/GameObject/GameObject.h"
#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
BaseCollider::BaseCollider(GameObject* own, ColliderType type, int ID, bool isMain, DirectX::SimpleMath::Vector3 center)
	: ICollider(own, ID, isMain)
	, m_type{ type }
	, m_boundingBox{ {0, 0, 0}, {0, 0, 0} }
	, m_localCenterPos{ center }
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
