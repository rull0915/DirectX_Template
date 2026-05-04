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

#include "GameLib/Scene/Scene.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
GameObject::GameObject()
    : m_pTransform{ std::make_unique<Transform>(this) }
    , m_pReserveRigidBody{}
    , m_pRigidBody{}
    , m_pReserves{}
    , m_pComponents{}
    , m_isActive{ true }
    , m_isDead{ false }
    , m_pScene{ nullptr }
{
    m_pTransform->UpdateCache();
}

void GameObject::BaseUpdate(float elapsedTime)
{
    // 派生クラスの更新処理
    Update(elapsedTime);
}

void GameObject::BaseRender(Renderer& renderer)
{
    CollidersDebugDraw(renderer, 0x00FF00, false);
}

void GameObject::BaseFinalize()
{
    // コンポーネント削除
    RemoveComponents();
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

/// <summary>
/// コンポーネントをすべて削除する関数
/// </summary>
void GameObject::RemoveComponents()
{
    // トランスフォーム
    m_pTransform = nullptr;

    // RigidBodyをマネージャーから削除
    if (m_pScene) m_pScene->UnRegsiterComponent(m_pRigidBody.get());
    m_pRigidBody = nullptr;

    // その他コンポーネントもマネージャーから削除
    for (auto& component : m_pComponents)
    {
        if (m_pScene) m_pScene->UnRegsiterComponent(component.get());
    }

    // 全削除
    m_pComponents.clear();
}

void GameObject::RegisterComponentToScene(BaseComponent* component)
{
    if (m_pScene) m_pScene->RegisterComponent(component);
}

void GameObject::RegisterComponents()
{
    // リジッドボディ
    if (m_pReserveRigidBody)
    {
        m_pScene->RegisterComponent(m_pReserveRigidBody.get());

        m_pRigidBody = std::move(m_pReserveRigidBody);

        m_pReserveRigidBody = nullptr;
    }

    // その他コンポーネント
    for (auto& component : m_pReserves)
    {
        m_pScene->RegisterComponent(component.get());

        m_pComponents.push_back(std::move(component));
    }

    m_pReserves.clear();
}
