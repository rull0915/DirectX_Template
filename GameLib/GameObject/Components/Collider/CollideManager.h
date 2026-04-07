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

#include "BaseCollider.h"
#include "CheckHit/CheckHit.h"

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

    // 管理しているコライダー
    std::vector<BaseCollider*> m_colliders;

    // レイヤー管理
    std::map<std::pair<std::string, std::string>, bool> m_layer;

private:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    CollideManager() 
        : m_colliders{}
    {
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
        m_colliders.push_back(collide);
    }

    // コライダーの削除
    void RemoveCollide(BaseCollider* collide)
    {
        m_colliders.erase(
            std::remove(m_colliders.begin(), m_colliders.end(), collide),
            m_colliders.end()
        );
    }

    /// <summary>
    /// レイヤー同士の衝突の有無を管理する関数
    /// </summary>
    /// <param name="layerA">レイヤー名</param>
    /// <param name="layerB">相手のレイヤー名</param>
    /// <param name="isActive">アクティブ状況</param>
    void SetCollideActive(const std::string& layerA, const std::string& layerB, bool isActive)
    {
        // 双方の条件を変更
        m_layer[std::make_pair(layerA, layerB)] = isActive;
        m_layer[std::make_pair(layerB, layerA)] = isActive;
    };

    // 全コライダーの衝突チェック
    void CheckHitAll();
};