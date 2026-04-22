//====================================================//
// ファイル名   : CollideManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/19
//
// 概要 : 衝突を管理するクラスです シングルトンで作成します
//
// 更新履歴 :
// 2026/03/19 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

#include <unordered_set>

#include "GameLib/GameObject/Components/RigidBody/3D/RigidBody.h"
#include "CheckHit/CheckHit.h"

#include "SpaceDivision/TreeManager.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//

// 衝突リスト格納用構造体
struct ContactPair
{
    BaseCollider* a;
    BaseCollider* b;

    bool operator==(const ContactPair& other) const {
        return (a == other.a && b == other.b) ||
            (a == other.b && b == other.a);
    }
};

class CollideManager
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 登録予約中のCollider
    std::vector<BaseCollider*> m_reserves;
    std::unordered_set<BaseCollider*> m_removeReserves;

    // 管理しているコライダー
    std::vector<BaseCollider*> m_colliders;
    std::vector<ObjectForTree*> m_treeObjects;

    // レイヤー管理
    std::vector<std::vector<bool>> m_layer;

    // 木構造
    TreeManager m_tree;

private:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    CollideManager() 
        : m_reserves{}
        , m_removeReserves{}
        , m_colliders{}
        , m_treeObjects{}
        , m_layer{}
        , m_tree{ { 128, 64, 128 }, 4, {0, 28, 0} }
    {
        m_layer.resize(100);
        for (auto& col : m_layer) col.resize(100);
    };
    ~CollideManager() = default;

public:
    // シングルトン化
    static CollideManager& Instance()
    {
        static CollideManager instance;

        return instance;
    }

    // コライダーの追加
    void AddCollide(BaseCollider* collide)
    {
        m_reserves.push_back(collide);
    }

    // コライダーの削除
    void RemoveCollide(BaseCollider* collide)
    {
        m_colliders.erase(
            std::remove(m_colliders.begin(), m_colliders.end(), collide),
            m_colliders.end()
        );
        for (auto& oft : m_treeObjects)
        {
            if (oft->m_pObject == collide)
            {
                std::remove(m_treeObjects.begin(), m_treeObjects.end(), oft);
                return;
            }
        }
    }

    /// <summary>
    /// レイヤー同士の衝突の有無を管理する関数
    /// </summary>
    void SetCollideActive(int layerA, int layerB, bool isActive)
    {
        // 双方の条件を変更
        m_layer[layerA][layerB] = isActive;
        m_layer[layerB][layerA] = isActive;
    };

    // 登録予約済みのコライダーを追加する関数
    void AddReserved();

    // 全コライダーのキャッシュ更新
    void UpdateCaches();

    // 全コライダーの木構造空間での移動
    void MoveAllColliderOnTree();

    // 全コライダーの衝突チェック
    void CheckHitAll();

    // コライダーの衝突チェック
    void CheckHitPair(BaseCollider*, BaseCollider*);
};