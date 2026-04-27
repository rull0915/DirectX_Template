//====================================================//
// ファイル名   : PhysicsManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/08
//
// 概要 : 物理挙動管理クラスです
//
// 更新履歴 :
// 2026/04/08 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "GameLib/GameObject/Components/RigidBody/3D/RigidBody.h"

#include "../HitContact.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>

//====================================================//
// 前方宣言
//====================================================//
 
//====================================================//
// クラス宣言
//====================================================//
class PhysicsManager
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------

    // 重力
    DirectX::SimpleMath::Vector3 m_gravityVec = DirectX::SimpleMath::Vector3{ 0, -1.0f, 0 };    // 向き
    float m_gravityPower = 20;  // 強さ

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 登録予約中のRigidBody
    std::vector<RigidBody*> m_reserves;
    std::unordered_set<RigidBody*> m_removeReserves;

    // 登録されているRigidBody
    std::vector<RigidBody*> m_rigidBodies;

    // 衝突情報
    std::vector<HitContact> m_contacts;
    std::unordered_map<ObjectPair, HitContact, ObjectPairHash> m_contactMap;

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
private:
    PhysicsManager();
public:
    ~PhysicsManager();
    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
public:
    static PhysicsManager& Instance()
    {
        static PhysicsManager instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 登録予約
    void AddRigidBody(RigidBody* r) { m_reserves.push_back(r); }
    void RemoveRigidBody(RigidBody* r) { m_removeReserves.insert(r); }

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    std::unordered_map<ObjectPair, HitContact, ObjectPairHash>& GetHitList() { return m_contactMap; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    // 予約済みポインタの追加
    void AddReserved()
    {
        for (auto p : m_reserves)
        {
            m_rigidBodies.push_back(p);
        }

        m_reserves.clear();
    }

    // 予約済みポインタの削除
    void RemoveReserved()
    {
        for (int i = 0; i < m_rigidBodies.size(); i++)
        {
            // もし削除リストに含まれていたら
            if (m_removeReserves.find(m_rigidBodies[i]) != m_removeReserves.end())
            {
                // 削除
                m_removeReserves.erase(m_rigidBodies[i]);
                m_rigidBodies.erase(m_rigidBodies.begin() + i);

                --i;
            }
        }

        m_removeReserves.clear();
    }

    // 衝突後の補正
    void HittedCorrection();

    void PositionCorrection(HitContact& contact);
    void VelocityCorrection(HitContact& contact);
};