#include "pch.h"
#include "GameObject.h"

#include "../Components/Collider/CollideManager.h"      // マネージャー

GameObject::~GameObject()
{
	// 全てのコライダーを削除
	for (auto& collider : m_pColliders)
	{
		CollideManager::Instance().RemoveCollide(collider.get());
	}
}

void GameObject::AllCollideAccept()
{
	for (const auto& col : m_pColliders)
	{
		CollideManager::Instance().AddCollide(col.get());
	}
}
