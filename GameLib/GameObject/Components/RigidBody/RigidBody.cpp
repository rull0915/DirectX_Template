#include "pch.h"
#include "RigidBody.h"

RigidBody::RigidBody(GameObject* own)
    : BaseComponent(own, MAIN_RIGIDBODY, true)
    , m_mass{ 1.0f }
    , m_isStatic{ false }
{
}
