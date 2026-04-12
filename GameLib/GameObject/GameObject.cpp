//====================================================//
// ファイル名  : GameObject.cpp
// 概要        : ゲームオブジェクト基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "GameObject.h"

#include "GameLib/GameMath/Color/ColorLib.h"

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
    m_debugColor = 0x00FF00;

    if (m_pRigidBody)
    {
        float mass = m_pRigidBody->GetMass();

        float ratio = (mass / 5.0f) * 0.5f + 0.5f;

        int h = m_pRigidBody->m_isSleep ? 200 : 20;

        m_debugColor = MyColor::HSVToRGB(h, 255, ratio * 255);
    }
}

void GameObject::BaseRender()
{
    CollidersDebugDraw(m_debugColor, false);
}

void GameObject::BaseFinalize()
{
    // トランスフォームの破棄
//    m_pTransform = nullptr;
}

/// <summary>
/// コライダーのデバッグ描画をする関数
/// </summary>
void GameObject::CollidersDebugDraw(int color, bool drawBoudingBox)
{
    static std::vector<BaseCollider*> colliders;

    GetComponents<BaseCollider>(colliders);

    for (auto& col : colliders)
    {
        col->DebugDraw(color);

        if (drawBoudingBox) col->DebugDrawAABB(color);
    }
}

void GameObject::BaseOnCollision(BaseCollider* other)
{
    // 必ず行う処理
    // m_debugColor = 0xFF0000;

    // 派生クラスの衝突時処理
    OnCollision(other);
}
