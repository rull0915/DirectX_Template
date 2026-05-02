//====================================================//
// ファイル名   : PrimitiveRenderer.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/29
//
// 概要 : プリミティブの描画を担当するクラス
//
// 更新履歴 :
// 2026/04/29 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <PrimitiveBatch.h>
#include <CommonStates.h>
#include <Effects.h>

#include "../../RenderStateCache.h"
#include "../IRenderer.h"

//====================================================//
// 前方宣言
//====================================================//
class Renderer;

//====================================================//
// クラス宣言
//====================================================//
class PrimitiveRenderer : public IRenderer
{
private:

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 所有者のRenderer
    Renderer& m_renderer;

    // 描画の状態
    RenderStateCache& m_renderState;

    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;	// プリミティブバッチ

    std::unique_ptr<DirectX::BasicEffect> m_basicEffect;	// ベーシックエフェクト

    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;


    // 開始済みフラグ
    bool m_isStarted;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    PrimitiveRenderer(Renderer& renderer);
    ~PrimitiveRenderer() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void Initialize() override;
    void Start() override ;
    void End() override;

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------
    void CheckChangeState();

public:

    //-----------------------------------------------------
    // 描画関数
    //-----------------------------------------------------

    // ------ Triangle ------ //

    void DrawTriangle	// 三角形描画関数 
    (
        DirectX::VertexPositionColor v1,	// 頂点情報
        DirectX::VertexPositionColor v2,	// 頂点情報
        DirectX::VertexPositionColor v3,	// 頂点情報
        bool fillFlag
    );
    void DrawTriangle	// 短縮版
    (
        DirectX::SimpleMath::Vector3 p1,
        DirectX::SimpleMath::Vector3 p2,
        DirectX::SimpleMath::Vector3 p3,
        int color, bool fillFlag
    );

    // -------- Rect -------- //
    
   void DrawRect
    (
        DirectX::VertexPositionColor v1,	// 頂点情報
        DirectX::VertexPositionColor v2,	// 頂点情報
        DirectX::VertexPositionColor v3,	// 頂点情報
        DirectX::VertexPositionColor v4,	// 頂点情報
        bool fillFlag
    );

   void DrawRect	// 短縮版
    (
        DirectX::SimpleMath::Vector3 p1,
        DirectX::SimpleMath::Vector3 p2,
        DirectX::SimpleMath::Vector3 p3,
        DirectX::SimpleMath::Vector3 p4,
        int color, bool fillFlag
    );
    
    // -------- Line -------- //
     
    void DrawLine
    (
        DirectX::VertexPositionColor v1,	// 頂点情報
        DirectX::VertexPositionColor v2		// 頂点情報
    );
    void DrawLine
    (
        DirectX::SimpleMath::Vector3 start,
        DirectX::SimpleMath::Vector3 end,
        int color
    );
     
    // -------- Circle -------- //

    void DrawCircle
    (
        DirectX::SimpleMath::Vector3 centerPos,
        DirectX::SimpleMath::Vector3 normal,
        float radius,
        int division,
        int color,
        bool fillFlag
    );

    // ---------- Arc ---------- //

    void DrawArc(
        const DirectX::SimpleMath::Vector3& center,
        DirectX::SimpleMath::Vector3 vStart, // 開始方向（正規化されている前提）
        DirectX::SimpleMath::Vector3 vEnd,   // 終了方向（正規化されている前提）
        int segments,
        float radius,
        int color,
        bool fillFlag
    );

    // ---------- index ------------ //

    void DrawIndex(
        D3D11_PRIMITIVE_TOPOLOGY topology,
        uint16_t* indexes,
        uint16_t indexCount,
        DirectX::VertexPositionColor* vertexes,
        uint16_t vertexCount
    );
};
