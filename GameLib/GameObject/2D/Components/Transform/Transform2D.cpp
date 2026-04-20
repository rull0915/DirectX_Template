//====================================================//
// ファイル名  : Transform2D.cpp.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要        :  トランスフォームコンポーネント
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "Transform2D.h"
#include "GameLib/GameObject/2D/GameObject2D.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

Transform2D::Transform2D(GameObject2D* own)
    : BaseComponent2D(own, MAIN_TRANSFORM_2D, true)
    , m_localPosition{ 0, 0 }
    , m_localRotation{ 0 }
    , m_localScale{ 1, 1 }
    , m_isDirty{ true }
    , m_pParent{ nullptr }
    , m_children{}
    , m_changeCache{ false }
{
}

Transform2D::~Transform2D()
{
    // 親がいれば親から自分を削除
    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
    }
    // 子供がいれば解放
    if (m_children.size() > 0)
    {
        RemoveChildren();
    }
}

/// <summary>
/// 親を設定する関数
/// </summary>
/// <param name="parent"></param>
void Transform2D::SetParent(Transform2D* parent)
{
    // 別の親に所属していたら
    if (m_pParent) m_pParent->RemoveChild(this);    // 親から自分を削除

    UpdateCache();

    m_localPosition = m_cache.position;
    m_localRotation = m_cache.rotation;
    m_localScale = m_cache.scale;

    m_pParent = parent;

    // 新しい親に自分を追加
    if (m_pParent) m_pParent->AddChild(this);

    // 変化フラグを立てる
    SetDirty();
}

/// <summary>
/// 全ての子供を解放する関数
/// </summary>
void Transform2D::RemoveChildren()
{
    // 現在の子リストをコピーする
    auto tempChildren = m_children;

    // コピーしたリストを使って処理
    for (auto* child : tempChildren)
    {
        child->SetParent(nullptr);
    }
    m_children.clear();
}

void Transform2D::UpdateCache() const
{
    // 親がいる場合
    if (m_pParent) {
        
        // 親の各種値を取得
        DirectX::SimpleMath::Vector2 pPos = m_pParent->GetWorldPosition();
        float pRot = m_pParent->GetWorldRotation();
        DirectX::SimpleMath::Vector2 pX = m_pParent->GetRight();
        DirectX::SimpleMath::Vector2 pY = m_pParent->GetUp();
        DirectX::SimpleMath::Vector2 pSc = m_pParent->GetWorldScale();

        // 親の値を使用しワールド座標系での情報を算出
        DirectX::SimpleMath::Vector2 difference = m_localPosition * pSc;

        m_cache.position = pPos + pX * difference.x + pY * difference.y;
        m_cache.rotation = pRot + m_localRotation;
        m_cache.scale = pSc * m_localScale;
    }
    else {
        m_cache.position = m_localPosition;
        m_cache.rotation = m_localRotation;
        m_cache.scale = m_localScale;
    }

    // フラグのリセット
    m_isDirty = false;
}

/// <summary>
/// ワールド座標系での座標を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector2 Transform2D::GetWorldPosition() const
{
    return m_cache.position;
}

/// <summary>
/// ワールド座標系での回転を返す関数
/// </summary>
/// <returns></returns>
const float Transform2D::GetWorldRotation() const
{
    return m_cache.rotation;
}

/// <summary>
/// ワールド座標系でのスケールを返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector2 Transform2D::GetWorldScale() const
{
    return m_cache.scale;
}

void Transform2D::SetWorldPosition(const DirectX::SimpleMath::Vector2 pos)
{
    if (m_pParent == nullptr) {
        // 親がいなければワールド＝ローカル
        m_localPosition = pos;
    }
    else {
        // 親がいる場合、ローカル座標に変換して保存

        // 親のワールド位置との差分をとる
        DirectX::SimpleMath::Vector2 diff = pos - m_pParent->GetWorldPosition();

        // 親のRight, Upベクトルを使って、diff を親の基底空間に分解する
        // (行列を使わない逆変換: 内積を利用)
        DirectX::SimpleMath::Vector2 pX = m_pParent->GetRight();
        DirectX::SimpleMath::Vector2 pY = m_pParent->GetUp();

        // 分解された座標
        float localX = diff.Dot(pX);
        float localY = diff.Dot(pY);

        // 親のスケールで割ってローカル位置に戻す
        DirectX::SimpleMath::Vector2 pSc = m_pParent->GetWorldScale();
        m_localPosition.x = localX / pSc.x;
        m_localPosition.y = localY / pSc.y;
    }
    // 行列の再計算フラグを立てる
    SetDirty();
}

DirectX::SimpleMath::Vector2 Transform2D::TransformForWorld(DirectX::SimpleMath::Vector2 point) const
{
    // 値を使用しワールド座標系での情報を算出
    DirectX::SimpleMath::Vector2 difference = point * GetWorldScale();
    return GetWorldPosition() + GetRight() * difference.x + GetUp() * difference.y;
}

/// <summary>
/// Dirtyフラグをセットする関数
/// </summary>
void Transform2D::SetDirty()
{
    if (m_isDirty) return;
    m_isDirty = true;

    // 子オブジェクトのフラグもセット
    for (auto* child : m_children) child->SetDirty();

    // コライダーのフラグもセット
    for (auto col : GetOwn()->GetComponents<BaseCollider2D>()) col->SetDirty();
}
