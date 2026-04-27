//====================================================//
// ファイル名   : PhysicsManager2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要 : 2D版物理挙動管理クラスです
//
// 更新履歴 :
// 2026/04/16 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "GameLib/GameObject/Components/RigidBody/2D/RigidBody2D.h"
#include "../HitContact.h"

#include <vector>
#include <unordered_set>

//====================================================//
// 前方宣言
//====================================================//

//====================================================//
// クラス宣言
//====================================================//
class PhysicsManager2D
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------

    // 重力
    DirectX::SimpleMath::Vector2 m_gravityVec = DirectX::SimpleMath::Vector2{ 0, -1.0f };    // 向き
    float m_gravityPower = 20;  // 強さ

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 登録予約中のRigidBody
    std::vector<RigidBody2D*> m_reserves;
    std::unordered_set<RigidBody2D*> m_removeReserves;

    // 登録されているRigidBody2D
    std::vector<RigidBody2D*> m_rigidBodies;

    // 衝突情報
    std::vector<HitContact2D> m_contacts;
    std::unordered_map<ObjectPair, HitContact2D, ObjectPairHash> m_contactMap;

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
private:
    PhysicsManager2D();
public:
    ~PhysicsManager2D();
    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
public:
    static PhysicsManager2D& Instance()
    {
        static PhysicsManager2D instance;
        return instance;
    }

    // 更新処理
    void Update(float elapsedTime);

    // 登録予約
    void AddRigidBody(RigidBody2D* r) { m_reserves.push_back(r); }
    void RemoveRigidBody(RigidBody2D* r) { m_removeReserves.insert(r); }

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    std::unordered_map<ObjectPair, HitContact2D, ObjectPairHash>& GetHitList() { return m_contactMap; }

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

    void PositionCorrection(HitContact2D& contact);
    void VelocityCorrection(HitContact2D& contact);
};