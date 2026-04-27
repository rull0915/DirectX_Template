//====================================================//
// ファイル名   : ICollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/22
//
// 概要 : コライダーインターフェース
//
// 更新履歴 :
// 2026/04/22 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseComponent.h"
#include "../Transform/Transform.h"
#include "PhysicsMaterial.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class ICollider : public BaseComponent
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 自身のレイヤー
    int m_layerNum;

    // 親のトランスフォーム
    Transform* m_pTransform;

    // トリガーフラグ
    bool m_isTrigger;

    // 変更済みフラグ
    mutable bool m_isChanged;

    // 値が変更されているかのフラグ
    mutable bool m_isDirty;

    // 最新のバージョン
    mutable uint32_t m_latestVersion;

    // 物理マテリアル
    PhysicsMaterial* m_physicsMaterial;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    ICollider(GameObject* own, int id, bool isMain);
    virtual ~ICollider() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
    virtual void DebugDraw(int color) const = 0;
    virtual void DebugDrawAABB(int color) const = 0;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    inline int GetLayer() const { return m_layerNum; }

    inline bool IsTrigger() const { return m_isTrigger; }
    inline bool IsChanged() const { return m_isChanged; }
    inline bool IsDirty() const { return m_isDirty || m_pTransform->GetVersion() != m_latestVersion; }

    inline Transform* GetTransform() const { return m_pTransform; }
    inline const PhysicsMaterial& GetPhysicsMaterial() const 
    {
        return (m_physicsMaterial ? *m_physicsMaterial : Physics::DEFAULT_MATERIAL); 
    }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetLayer(int num) { m_layerNum = std::min(num, 99); }
    void SetDirty() { m_isDirty = true; };
    void SetTrigger(bool frag) { m_isTrigger = frag; }
    void ResetChangeFrag() { m_isChanged = false; }
    void SetPhysicsMaterial(PhysicsMaterial* mat) { m_physicsMaterial = mat; }

protected:

    inline void ResetDirty() const { m_isDirty = false; }
    inline void SetChanged(bool frag) const { m_isChanged = frag; }
    inline void ApplyVersion() const { m_latestVersion = m_pTransform->GetVersion(); }
};