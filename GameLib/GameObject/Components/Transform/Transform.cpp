//====================================================//
// ファイル名  : Transform.cpp
// 概要        :  トランスフォームコンポーネント
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "Transform.h"
#include "../../GameObject.h"

using namespace DirectX;

//====================================================//
// 関数の実体宣言
//====================================================//

Transform::Transform(GameObject* own)
    : BaseComponent(own, MAIN_TRANSFORM, true)
    , m_localPosition{ 0, 0, 0 }
    , m_localRotation{}
    , m_localScale{ 1, 1, 1 }
    , m_worldMatrix{}
    , m_isDirty{ true }
    , m_pParent{ nullptr }
    , m_children{}
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

void Transform::UpdateCache() const
{
    if (!m_isDirty) return;

    using namespace DirectX::SimpleMath;

    // 各ワールド成分を親から継承
    if (m_pParent) {
        m_worldScaleMatrix = Matrix::CreateScale(m_localScale) * m_pParent->GetWorldScaleMatrix();
        m_worldRotationMatrix = Matrix::CreateFromQuaternion(m_localRotation) * m_pParent->GetWorldRotationMatrix();

        // 親のワールド行列を使用して座標変換
        Vector3 worldPos = Vector3::Transform(m_localPosition, m_pParent->GetWorldMatrix());
        m_worldPositionMatrix = Matrix::CreateTranslation(worldPos);
    }
    else {
        m_worldScaleMatrix = Matrix::CreateScale(m_localScale);
        m_worldRotationMatrix = Matrix::CreateFromQuaternion(m_localRotation);
        m_worldPositionMatrix = Matrix::CreateTranslation(m_localPosition);
    }

    // --- ワールド行列の更新 --- //
    m_worldMatrix = m_worldScaleMatrix * m_worldRotationMatrix * m_worldPositionMatrix;

    // フラグのリセット
    m_isDirty = false;
}

/// <summary>
/// ワールド座標系での座標を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector3 Transform::GetWorldPosition() const
{
    UpdateCache();

    // 最新のワールド行列を取得
    auto& world = GetWorldPositionMatrix();

    // 位置成分を返す
    return world.Translation();
}

/// <summary>
/// ワールド座標系での回転を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Quaternion Transform::GetWorldRotation() const
{
    UpdateCache();

    // 回転行列からクォータニオンを作る
    return DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_worldRotationMatrix);
}

/// <summary>
/// ワールド座標系でのオイラー角を返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector3 Transform::GetWorldEulerAngle() const
{
    UpdateCache();
    return GetWorldRotation().ToEuler();
}

/// <summary>
/// ワールド座標系でのスケールを返す関数
/// </summary>
/// <returns></returns>
const DirectX::SimpleMath::Vector3 Transform::GetWorldScale() const
{
    UpdateCache();
    return { m_worldScaleMatrix._11, m_worldScaleMatrix._22, m_worldScaleMatrix._33 };
}

/// <summary>
/// ワールド行列を返す関数
/// </summary>
/// <returns></returns>
DirectX::SimpleMath::Matrix& Transform::GetWorldMatrix() const
{
    UpdateCache();
    return m_worldMatrix;
}

DirectX::SimpleMath::Matrix& Transform::GetWorldPositionMatrix() const
{
    UpdateCache();
    return m_worldPositionMatrix;
}

DirectX::SimpleMath::Matrix& Transform::GetWorldRotationMatrix() const
{
    UpdateCache();
    return m_worldRotationMatrix;
}

DirectX::SimpleMath::Matrix& Transform::GetWorldScaleMatrix() const
{
    UpdateCache();
    return m_worldScaleMatrix;
}

void Transform::SetWorldPosition(const DirectX::SimpleMath::Vector3 pos)
{
    if (m_pParent == nullptr) {
        // 親がいなければワールド＝ローカル
        m_localPosition = pos;
    }
    else {
        // 親がいる場合、親の逆行列をかけて「ローカル座標」に変換して保存
        SimpleMath::Matrix invParentWorld = m_pParent->GetWorldMatrix().Invert();
        m_localPosition = SimpleMath::Vector3::Transform(pos, invParentWorld);
    }
    // 行列の再計算フラグを立てる
    SetDirty();
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

    // コライダーのフラグもセット
    for (auto col : GetOwn()->GetComponents<BaseCollider>()) col->SetDirty();
}