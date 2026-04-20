//====================================================//
// ファイル名  : BaseCollider2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要        :  2Dコライダー基底クラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "BaseCollider2D.h"

#include "GameLib/GameObject/2D/Managers/CollideManager2D.h"

#include "GameLib/MyRenderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

BaseCollider2D::BaseCollider2D(GameObject2D* own, ColliderType2D type, int ID, bool isMain, DirectX::SimpleMath::Vector2 center)
	: BaseComponent2D(own, ID, isMain)
	, m_layerNum{ -1 }
	, m_type{ type }
	, m_localCenterPos{ center }
	, m_pTransform{ own->GetComponent<Transform2D>() }
	, m_boundingBox{}
	, m_isTrigger{ false }
	, m_isChanged{ false }
	, m_isDirty{ true }
	, m_worldCenterPos{}
{
	// 自身をマネージャーに登録
	CollideManager2D::Instance().AddCollide(this);
}

void AABB2D::DebugDraw(int color) const
{
	MyRenderer::DrawLine({ min.x, min.y, 0 }, { max.x, min.y, 0 }, color);
	MyRenderer::DrawLine({ min.x, max.y, 0 }, { max.x, max.y, 0 }, color);
	MyRenderer::DrawLine({ min.x, min.y, 0 }, { min.x, max.y, 0 }, color);
	MyRenderer::DrawLine({ max.x, min.y, 0 }, { max.x, max.y, 0 }, color);
}
