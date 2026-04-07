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
    };

    mutable WorldCache m_cache;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BoxCollider(GameObject* own)
        : BaseCollider(own, ColliderType::Box, SUB_COLLIDER_BOX, false) 
        , m_localSize{ 1, 1, 1 }
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
        UpdateCache();
        return m_cache.xAxis;
    }
    DirectX::SimpleMath::Vector3 GetYAxis() const
    {
        UpdateCache();
        return m_cache.yAxis;
    }
    DirectX::SimpleMath::Vector3 GetZAxis() const
    {
        UpdateCache();
        return m_cache.zAxis;
    }

    DirectX::SimpleMath::Vector3 GetSize() const
    {
        UpdateCache();
        return m_cache.scale;
    }
    DirectX::SimpleMath::Vector3 GetHalfSize() const
    {
        UpdateCache();
        return m_cache.scale * 0.5f;
    }
    DirectX::SimpleMath::Matrix GetLocalMatrix() const
    {
        UpdateCache();
        return m_cache.localMatrix;
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