//====================================================//
// ファイル名   : ModelComponent.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/05/03
//
// 概要 :
//
// 更新履歴 :
// 2026/05/03 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseRenderer.h"
#include "GameLib/Resources/ResourceManager.h"
#include <Model.h>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class ModelComponent : public BaseRenderer
{
    // ----------------------------------------------------
    // 定数宣言
    // ----------------------------------------------------
public:
    static constexpr int TYPE_ID = SUB_RENDERER_MODEL;
    static constexpr bool IS_MAIN = false;

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------
    DirectX::Model* m_model;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    ModelComponent(GameObject* own)
        : BaseRenderer(own, SUB_RENDERER_MODEL, false)
        , m_model{ nullptr }
    {
    };
    ~ModelComponent() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // 描画関数
    void Draw(Renderer& renderer) override
    {
        // モデルがnullなら描画しない
//        assert(m_model && "Model not found");
        if (!m_model) return;

        // トランスフォームからworld行列を取得
        const DirectX::SimpleMath::Matrix& world = GetTransform()->GetWorldMatrix();

        // 行列を使用しモデルを描画
        renderer.Draw().Model(m_model, world);
    }

    // モデルをセットする関数
    void SetModel(const std::string& keyName)
    {
        m_model = ResourceManager::Instance().GetModel(keyName);
    }
};