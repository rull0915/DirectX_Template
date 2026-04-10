//====================================================//
// ファイル名   : BoxCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : ボックスのコライダー
//
// 更新履歴 :
// 2026/03/18 新規作成
// 2026/04/03 設計を変更
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseCollider.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class BoxCollider : public BaseCollider
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = SUB_COLLIDER_BOX;
    static constexpr bool IS_MAIN = false;

private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // ボックスのローカルでのサイズ
    DirectX::SimpleMath::Vector3 m_localSize;

    // 計算済みワールド情報
    struct WorldCache {
        DirectX::SimpleMath::Vector3 xAxis;
        DirectX::SimpleMath::Vector3 yAxis;
        DirectX::SimpleMath::Vector3 zAxis;

        DirectX::SimpleMath::Vector3 scale;

        DirectX::SimpleMath::Matrix localMatrix;
        DirectX::SimpleMath::Matrix localMatrixInverse;
    };

    mutable WorldCache m_cache;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BoxCollider(GameObject* own, DirectX::SimpleMath::Vector3 size = { 1, 1, 1 }, DirectX::SimpleMath::Vector3 pos = { 0, 0, 0 })
        : BaseCollider(own, ColliderType::Box, SUB_COLLIDER_BOX, false, pos) 
        , m_localSize{ size }
        , m_cache{}
    {
    };
    ~BoxCollider() = default;

    void UpdateCache() const override;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    // 各軸ベクトルを取得する関数
    DirectX::SimpleMath::Vector3 GetXAxis() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.xAxis;
    }
    DirectX::SimpleMath::Vector3 GetYAxis() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.yAxis;
    }
    DirectX::SimpleMath::Vector3 GetZAxis() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.zAxis;
    }

    DirectX::SimpleMath::Vector3 GetSize() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.scale;
    }
    DirectX::SimpleMath::Vector3 GetHalfSize() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.scale * 0.5f;
    }
    DirectX::SimpleMath::Matrix GetLocalMatrix() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.localMatrix;
    }
    DirectX::SimpleMath::Matrix GetLocalMatrixInverse() const
    {
        if(m_isDirty) UpdateCache();
        return m_cache.localMatrixInverse;
    }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetLocalSize(DirectX::SimpleMath::Vector3 scale)
    {
        m_localSize = scale;
        SetDirty();
    }

    // 描画関数
    void DebugDraw(int color) const override;
};