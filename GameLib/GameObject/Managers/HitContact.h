//====================================================//
// ファイル名   : HitContact.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/24
//
// 概要 : 衝突情報をまとめた構造体
//
// 更新履歴 :
// 2026/04/24 新規作成
//====================================================//

#pragma once

class GameObject;
class BaseCollider;
class BaseCollider2D;
class RigidBody;
class RigidBody2D;

//====================================================//
// 構造体宣言
//====================================================//

struct ObjectPair
{
    GameObject* a;
    GameObject* b;

public:
    ObjectPair(GameObject* pA, GameObject* pB)
    {
        a = pA > pB ? pA : pB;
        b = pA < pB ? pA : pB;
    }

    bool operator==(const ObjectPair& other) const
    {
        return (a == other.a && b == other.b) ||
            (a == other.b && b == other.a);
    }
};

struct ObjectPairHash
{
    size_t operator()(const ObjectPair& p) const
    {
        size_t h1 = std::hash<GameObject*>()(p.a);
        size_t h2 = std::hash<GameObject*>()(p.b);
        return h1 ^ (h2 << 1);
    }
};

struct HitContact
{
    // 衝突した2つのオブジェクト
    GameObject* a;
    GameObject* b;

    // 衝突したリジッドボディ
    RigidBody* aRigid;
    RigidBody* bRigid;

    // 衝突したコライダー
    BaseCollider* aCol;
    BaseCollider* bCol;

    // トリガーフラグ
    bool isTrigger;

    // スタティックかどうか
    bool aIsStatic;
    bool bIsStatic;

    // 衝突法線
    DirectX::SimpleMath::Vector3 normal;

    // めり込み量
    float penetration;

    // 衝突点(未使用)
    DirectX::SimpleMath::Vector3 point;
};

struct HitContact2D
{
    // 衝突した2つのオブジェクト
    GameObject* a;
    GameObject* b;

    // 衝突したリジッドボディ
    RigidBody2D* aRigid;
    RigidBody2D* bRigid;

    // 衝突したコライダー
    BaseCollider2D* aCol;
    BaseCollider2D* bCol;

    // トリガーフラグ
    bool isTrigger;

    // スタティックかどうか
    bool aIsStatic;
    bool bIsStatic;

    // 衝突法線
    DirectX::SimpleMath::Vector2 normal;

    // めり込み量
    float penetration;

    // 衝突点(未使用)
    DirectX::SimpleMath::Vector2 point;
};
