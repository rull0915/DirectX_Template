//====================================================//
// ファイル名  : GameObject.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/01
//
// 概要        : ゲームオブジェクト基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "GameObject.h"

#include "GameLib/GameMath/Color/ColorLib.h"
#include "GameLib/Input/KeyInput.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
GameObject::GameObject()
    : m_pComponents{}
    , m_pTransform{ std::make_unique<Transform>(this) }
    , m_isActive{ true }
    , m_isDead{ false }
    , m_debugColor{ 0x00FF00 }
{
    m_pTransform->UpdateCache();
}

void GameObject::BaseUpdate(float elapsedTime)
{
    elapsedTime;

    m_debugColor = 0x00FF00;

    if (m_pRigidBody)
    {
        float mass = m_pRigidBody->GetMass();

        int h = m_pRigidBody->IsSleep() ? (KeyInput::GetKey(DirectX::Keyboard::Keys::F2) ? 200 : 120) : (KeyInput::GetKey(DirectX::Keyboard::Keys::F2) ? 20 : 120);

        m_debugColor = MyColor::HSVToRGB(h, 255, 255);
    }
}

void GameObject::BaseRender(Renderer& renderer)
{
    CollidersDebugDraw(renderer, m_debugColor, false);
//    CollidersDebugDraw(0x00FF00, false);
}

void GameObject::BaseFinalize()
{
    // トランスフォームの破棄
//    m_pTransform = nullptr;
}

/// <summary>
/// コライダーのデバッグ描画をする関数
/// </summary>
void GameObject::CollidersDebugDraw(Renderer& renderer, int color, bool drawBoundingBox)
{
    static std::vector<BaseCollider*> colliders;
    static std::vector<BaseCollider2D*> colliders2D;

    GetComponents<BaseCollider>(colliders);
    GetComponents<BaseCollider2D>(colliders2D);

    for (auto& col : colliders)
    {
        col->DebugDraw(renderer, color);

        if (drawBoundingBox) col->DebugDrawAABB(renderer, color);
    }

    for (auto& col : colliders2D)
    {
        col->DebugDraw(renderer, color);

        if (drawBoundingBox) col->DebugDrawAABB(renderer, color);
    }
}

void GameObject::BaseOnCollision2D(BaseCollider2D* other)
{
    OnCollision2D(other);
}

void GameObject::BaseOnTrigger2D(BaseCollider2D* other)
{
    OnTrigger2D(other);
}
