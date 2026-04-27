//====================================================//
// ファイル名  : ICollider.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/22
//
// 概要        : Colliderインターフェース
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//

#include "pch.h"
#include "ICollider.h"

#include "../../GameObject.h"

ICollider::ICollider(GameObject* own, int id, bool isMain)
	: BaseComponent(own, id, isMain)
	, m_pTransform{ own->GetComponent<Transform>() }
	, m_isTrigger{ false }
	, m_isDirty{ true }
	, m_isChanged{ false }
	, m_layerNum{ -1 }
	, m_latestVersion{ 1 }
, m_physicsMaterial{ nullptr }
{
}
