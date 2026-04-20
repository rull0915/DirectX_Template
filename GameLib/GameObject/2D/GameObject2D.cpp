//====================================================//
// ファイル名  : GameObject2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要        : 2D版ゲームオブジェクト基底クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "GameObject2D.h"

#include "GameLib/GameMath/Color/ColorLib.h"
#include "GameLib/Input/KeyInput.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
GameObject2D::GameObject2D()
    : m_pComponents{}
    , m_pTransform{ std::make_unique<Transform2D>(this) }
    , m_isActive{ true }
    , m_isDead{ false }
    , m_debugColor{ 0x00FF00 }
{
    m_pTransform->UpdateCache();
}

void GameObject2D::BaseUpdate(float elapsedTime)
{
    elapsedTime;

    m_debugColor = 0x00FF00;

    if (m_pRigidBody)
    {
        float mass = m_pRigidBody->GetMass();

        float ratio = (mass / 5.0f) * 0.5f + 0.5f;

        int h = m_pRigidBody->m_isSleep ? (KeyInput::GetKey(DirectX::Keyboard::Keys::F2) ? 200 : 120) : (KeyInput::GetKey(DirectX::Keyboard::Keys::F2) ? 20 : 120);

        m_debugColor = MyColor::HSVToRGB(h, 255, static_cast<int>(ratio * 255));
    }
}

void GameObject2D::BaseRender()
{
    CollidersDebugDraw(m_debugColor, false);
//    CollidersDebugDraw(0x00FF00, false);
}

void GameObject2D::BaseFinalize()
{
    // トランスフォームの破棄
//    m_pTransform = nullptr;
}

/// <summary>
/// コライダーのデバッグ描画をする関数
/// </summary>
void GameObject2D::CollidersDebugDraw(int color, bool drawBoudingBox)
{
    static std::vector<BaseCollider2D*> colliders;

    GetComponents<BaseCollider2D>(colliders);

    for (auto& col : colliders)
    {
        col->DebugDraw(color);

        if (drawBoudingBox) col->DebugDrawAABB(color);
    }
}

void GameObject2D::BaseOnCollision(BaseCollider2D* other)
{
    // 必ず行う処理
    m_debugColor = 0xFF0000;

    // 派生クラスの衝突時処理
    OnCollision(other);
}
