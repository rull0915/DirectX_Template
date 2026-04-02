//====================================================//
// ファイル名  : BaseCollider.cpp
// 概要        :  コライダーの基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "BaseCollider.h"

#include "CollideManager.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
BaseCollider::BaseCollider(GameObject* own, ColliderType type)
	: BaseComponent(own)
	, m_type{ type }
{
	// 自身をマネージャーに登録
	CollideManager::Instance().AddCollide(this);
}

BaseCollider::~BaseCollider()
{
	// 自身をマネージャーから削除
	CollideManager::Instance().RemoveCollide(this);
}
