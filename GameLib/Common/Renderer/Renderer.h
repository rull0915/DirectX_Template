//====================================================//
// ファイル名   : Renderer.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/29
//
// 概要 : 描画を統括するクラス
//
// 更新履歴 :
// 2026/04/29 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

// DirectXTK
#include <CommonStates.h>

// Renderers
#include "RenderStateCache.h"
#include "RenderProxy.h"

#include "EachRenderer/Primitive/PrimitiveRenderer.h"
#include "EachRenderer/Model/ModelRenderer.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class Renderer
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------
    
    // デバイス
    ID3D11Device* m_device;
    // コンテキスト
    ID3D11DeviceContext* m_nowContext;
    // コモンステート
    DirectX::CommonStates* m_states;

    // 描画ステート
    RenderStateCache m_drawState;
    // 描画仲介クラス
    RenderProxy m_proxy;

    // プリミティブ
    std::unique_ptr<PrimitiveRenderer> m_primitiveRenderer;
    // モデル
    std::unique_ptr<ModelRenderer> m_modelRenderer;

    // 現在描画しているレンダラー
    RendererType m_nowType;
    IRenderer* m_nowRenderer;

    // ----- フラグ ----- //

    // 描画開始フラグ
    bool m_isStarted;
    // 初期化フラグ
    bool m_isInitialized;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    Renderer();
    ~Renderer() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // 初期化
    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::CommonStates* state);

    // 描画開始
    void Start(ID3D11DeviceContext* context);

    // 描画終了
    void End();

    // 描画関数
    RenderProxy& Draw();

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    RenderStateCache& GetRenderState() { return m_drawState; }
    ID3D11Device* GetDevice() const { return m_device; }
    ID3D11DeviceContext* GetContext() const { return m_nowContext; }
    DirectX::CommonStates* GetStates() const { return m_states; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetWorld(DirectX::SimpleMath::Matrix world) { m_drawState.SetWorld(world); }
    void SetView(DirectX::SimpleMath::Matrix view) { m_drawState.SetView(view); }
    void SetProjection(DirectX::SimpleMath::Matrix proj) { m_drawState.SetProjection(proj); }
    void SetAlpha(float alpha) { m_drawState.SetAlpha(alpha); }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    /// <summary>
    /// 指定したタイプのレンダラーを返す関数
    /// </summary>
    IRenderer* GetRenderer(RendererType type)
    {
        // 指定した描画タイプのレンダラーを返す
        switch (type)
        {
        case RendererType::Primitive:
            return m_primitiveRenderer.get();
        case RendererType::Model:
            return m_modelRenderer.get();
        case RendererType::Sprite:
            break;
        default:
            break;
        }
    }

    /// <summary>
    /// レンダラーを変更する関数
    /// </summary>
    void SwitchRenderer(RendererType type)
    {
        // 変更がなければスキップ
        if (m_nowType == type) return;

        // 現在の描画の終了処理
        if (m_nowRenderer) m_nowRenderer->End();

        // レンダラーの変更
        m_nowType = type;
        m_nowRenderer = GetRenderer(type);

        // 次のレンダラーの開始処理
        m_nowRenderer->Start();
    }

    /// <summary>
    /// Proxyに渡す用の仲介関数
    /// </summary>
    static void SwitchRendererProxy(void* renderer, RendererType type)
    {
        static_cast<Renderer*>(renderer)->SwitchRenderer(type);
    }
};

