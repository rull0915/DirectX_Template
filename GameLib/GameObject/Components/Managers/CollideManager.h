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

#include <map>

#include "../RigidBody/RigidBody.h"
#include "../Collider/CheckHit/CheckHit.h"

#include "SpaceDivision/TreeManager.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
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

    // 衝突リスト保持用配列
    unsigned int m_colCount;
    std::vector<BaseCollider*> m_collideList;

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
        , m_collideList{}
        , m_layer{}
        , m_tree{ { 64, 64, 64 }, 6 }
        , m_colCount{ 0 }
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

    // 全コライダーの木構造空間での移動
    void MoveAllColliderOnTree();

    // 衝突リストの作成関数
    void MakeCollisionList();

    // 全コライダーの衝突チェック
    void CheckHitAll();
};