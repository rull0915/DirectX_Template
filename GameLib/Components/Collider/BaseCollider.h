//====================================================//
// ファイル名   : BaseCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : 衝突判定の基底クラス
//
// 更新履歴 :
// 2026/03/18 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//


//====================================================//
// 前方宣言
//====================================================//
class GameObject;

//====================================================//
// 列挙型宣言
//====================================================//
enum class ColliderType
{
    Sphere,
    Line,
    AABB,
    Capsule,
    Box,
};

//====================================================//
// 構造体宣言
//====================================================//
struct AABB
{
    DirectX::SimpleMath::Vector3 min;
    DirectX::SimpleMath::Vector3 max;

    AABB(DirectX::SimpleMath::Vector3 a, DirectX::SimpleMath::Vector3 b)
        : min{ a }
        , max{ b }
    {
    };
};

//====================================================//
// クラス宣言
//====================================================//
class BaseCollider
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 自分を所持するGameObjectのポインタ
    GameObject* m_own;

    // 自身のレイヤー名
    std::string m_layerName;

    // コライダーのタイプ
    ColliderType m_type;

    // 有効かどうか
    bool m_isActive;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BaseCollider(GameObject* own, ColliderType type)
        : m_own{ own }
        , m_layerName{ "" }
        , m_type{ type }
        , m_isActive{ true }
    {
    };
    virtual ~BaseCollider() = default;

    ColliderType GetType() const { return m_type; };

    GameObject* GetOwn() const { return m_own; }

    // Activeフラグの管理
    bool IsActive() const { return m_isActive; }
    void SetActive(bool f) { m_isActive = f; }

    void SetLayerName(const std::string& name) { m_layerName = name; }
    const std::string& GetLayerName() { return m_layerName; }

    // 自身を覆うAABBを取得する関数
    virtual AABB GetBoundingBox() const = 0;
};