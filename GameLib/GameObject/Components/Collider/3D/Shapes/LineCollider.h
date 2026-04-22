//====================================================//
// ファイル名   : LineCollider.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : 線のコライダー
//
// 更新履歴 :
// 2026/03/18 新規作成
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
class LineCollider : public BaseCollider
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 中心座標
    DirectX::SimpleMath::Vector3 m_start;
    DirectX::SimpleMath::Vector3 m_end;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    LineCollider(GameObject* own, DirectX::SimpleMath::Vector3 start, DirectX::SimpleMath::Vector3 end)
        : BaseCollider(own, ColliderType::Line, false, 0)
        , m_start{ start }
        , m_end{ end }
    {
    };
    ~LineCollider() = default;

    DirectX::SimpleMath::Vector3 GetStart() const { return m_start; };
    DirectX::SimpleMath::Vector3 GetEnd()   const { return m_end; };

    void SetStart(DirectX::SimpleMath::Vector3 start) { m_start = start; }
    void SetEnd(DirectX::SimpleMath::Vector3 end) { m_end = end; }

    void UpdateCache() const override {};
    void DebugDraw(int color) const override {};
};