//====================================================//
// ファイル名  : ModelRenderer.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/05/01
//
// 概要        :  モデルの描画をするクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "ModelRenderer.h"
#include "../../Renderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="renderer">レンダラー</param>
ModelRenderer::ModelRenderer(Renderer& renderer)
	: m_renderer{ renderer }
	, m_renderState{ renderer.GetRenderState() }
{
}

/// <summary>
/// 初期化関数
/// </summary>
void ModelRenderer::Initialize()
{
}

/// <summary>
/// 描画開始関数
/// </summary>
void ModelRenderer::Start()
{
}

/// <summary>
/// 描画終了関数
/// </summary>
void ModelRenderer::End()
{
}

/// <summary>
/// モデル
/// </summary>
/// <param name="model">モデルハンドル</param>
/// <param name="world">ワールド行列</param>
void ModelRenderer::DrawModel(DirectX::Model* model, DirectX::SimpleMath::Matrix world)
{
	if (!model) return;

	model->Draw(
		m_renderer.GetContext(), 
		*m_renderer.GetStates(), 
		world, 
		m_renderState.GetView(), 
		m_renderState.GetProjection()
	);
}
