//====================================================//
// ファイル名  : CollideEventSystem.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/26
//
// 概要        :  
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "CollideEventSystem.h"

#include "../../GameObject.h"

//====================================================//
// 関数の実体宣言
//====================================================//

void CollideEventSystem::CallCollideFunctions(std::unordered_map<ObjectPair, HitContact, ObjectPairHash>& nowMap)
{
    // Enter & Stay
    for (auto& pair : nowMap)
    {
        if (m_prevMap.count(pair.first) == 0)
            OnEnter(pair.second);
        else
            OnStay(pair.second);
    }

    // Exit
    for (auto& pair : m_prevMap)
    {
        if (nowMap.count(pair.first) == 0)
            OnExit(pair.second);
    }

    m_prevMap = nowMap;
}

void CollideEventSystem::OnEnter(HitContact& contact)
{
    if (contact.isTrigger)
    {
        contact.a->OnTriggerEnter(contact.bCol);
        contact.b->OnTriggerEnter(contact.aCol);
    }
    else
    {
        contact.a->OnCollisionEnter(contact.bCol);
        contact.b->OnCollisionEnter(contact.aCol);
    }
}

void CollideEventSystem::OnStay(HitContact& contact)
{
    if (contact.isTrigger)
    {
        contact.a->OnTriggerStay(contact.bCol);
        contact.b->OnTriggerStay(contact.aCol);
    }
    else
    {
        contact.a->OnCollisionStay(contact.bCol);
        contact.b->OnCollisionStay(contact.aCol);
    }
}

void CollideEventSystem::OnExit(HitContact& contact)
{
    if (contact.isTrigger)
    {
        contact.a->OnTriggerExit(contact.bCol);
        contact.b->OnTriggerExit(contact.aCol);
    }
    else
    {
        contact.a->OnCollisionExit(contact.bCol);
        contact.b->OnCollisionExit(contact.aCol);
    }
}
