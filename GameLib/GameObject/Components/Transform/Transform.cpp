#include "Transform.h"
//====================================================//
// ファイル名  : Transform.cpp
// 概要        :  トランスフォームコンポーネント
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

Transform::Transform(GameObject* own)
    : BaseComponent(own)
{
}

Transform::~Transform()
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
void Transform::SetParent(Transform* parent)
{
    // ワールド行列を取得
    auto& world = GetWorldMatrix();

    // 別の親に所属していたら
    if (m_pParent) m_pParent->RemoveChild(this);    // 親から自分を削除

    // 親のワールド行列を取得
    SimpleMath::Matrix parentWorld = (parent ? parent->GetWorldMatrix() : SimpleMath::Matrix::Identity);

    // 新しいローカル行列を算出
    SimpleMath::Matrix newLocal = world * parentWorld.Invert();

    // ローカル情報にセット
    SimpleMath::Vector3 s; SimpleMath::Quaternion r; SimpleMath::Vector3 t;
    newLocal.Decompose(s, r, t);

    m_localPosition = t;
    m_localRotation = r;
    m_localScale = s;

    m_pParent = parent;

    // 新しい親に自分を追加
    if (m_pParent) m_pParent->AddChild(this);

    // 変化フラグを立てる
    SetDirty();
}

/// <summary>
/// 全ての子供を解放する関数
/// </summary>
void Transform::RemoveChildren()
{
    // 全ての子オブジェクトを捜査
    for (auto& child : m_children)
    {
        child->SetParent(nullptr);
    }
}

/// <summary>
/// ワールド座標系での座標を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector3 Transform::GetWorldPosition()
{
    // 最新のワールド行列を取得
    auto& world = GetWorldMatrix();

    // 位置成分を返す
    return world.Translation();
}

/// <summary>
/// ワールド座標系での回転を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Quaternion Transform::GetWorldRotation()
{
    // 最新のワールド行列を取得
    auto& world = GetWorldMatrix();

    SimpleMath::Vector3 s; SimpleMath::Quaternion r; SimpleMath::Vector3 t;
    world.Decompose(s, r, t);
    return r;
}

/// <summary>
/// ワールド座標系でのオイラー角を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector3 Transform::GetWorldEulerAngle()
{
    // 最新のワールド行列を取得
    auto& world = GetWorldMatrix();

    SimpleMath::Vector3 s; SimpleMath::Quaternion r; SimpleMath::Vector3 t;
    world.Decompose(s, r, t);
    return r.ToEuler();
}

/// <summary>
/// ワールド座標系でのスケールを返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector3 Transform::GetWorldScale()
{
    // 最新のワールド行列を取得
    auto& world = GetWorldMatrix();

    SimpleMath::Vector3 s; SimpleMath::Quaternion r; SimpleMath::Vector3 t;
    world.Decompose(s, r, t);
    return s;
}

/// <summary>
/// ワールド行列を返す関数
/// </summary>
/// <returns></returns>
DirectX::SimpleMath::Matrix& Transform::GetWorldMatrix()
{
    if (m_isDirty) {
        // ローカル行列作成
        DirectX::SimpleMath::Matrix local =
            DirectX::SimpleMath::Matrix::CreateScale(m_localScale) *
            DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_localRotation) *
            DirectX::SimpleMath::Matrix::CreateTranslation(m_localPosition);

        // 親オブジェクトがあれば
        if (m_pParent) {
            // 親の行列と合成
            m_worldMatrix = local * m_pParent->GetWorldMatrix();
        }
        else {
            m_worldMatrix = local;
        }
        // フラグリセット
        m_isDirty = false;
    }
    return m_worldMatrix;
}

/// <summary>
/// Dirtyフラグをセットする関数
/// </summary>
void Transform::SetDirty()
{
    if (m_isDirty) return;
    m_isDirty = true;

    // 子オブジェクトのフラグもセット
    for (auto* child : m_children) child->SetDirty();
}