//====================================================//
// ファイル名  : IRigidBody.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/22
//
// 概要        :  RigidBodyインターフェース
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//

#include "pch.h"
#include "IRigidBody.h"

#include "../../GameObject.h"

IRigidBody::IRigidBody(GameObject* own, int id)
    : BaseComponent(own, id, true)
    , m_pTransform{ own->GetComponent<Transform>() }
    , m_mass{ 1.0f }
    , m_invMass{ 1.0f / m_mass }
    , m_friction{ 0.0f }
    , m_restitution{ 0.5f }
    , m_isStatic{ false }
    , m_useGravity{ true }
    , m_isSleep{ false }
    , m_stoppingTime{ 0 }
{
}
