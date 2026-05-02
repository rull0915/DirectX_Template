//====================================================//
// ファイル名  : SpriteRenderer.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/05/01
//
// 概要        :  スプライトの描画を担当する関数
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "SpriteRenderer.h"
#include "../../Renderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
SpriteRenderer::SpriteRenderer(Renderer& renderer)
	: m_renderer{ renderer }
	, m_renderState{ renderer.GetRenderState() }
{
}

/// <summary>
/// デストラクタ
/// </summary>
SpriteRenderer::~SpriteRenderer()
{
}

/// <summary>
/// 初期化関数
/// </summary>
void SpriteRenderer::Initialize()
{
	auto context = m_renderer.GetContext();

	// スプライトバッチの初期化
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
}

/// <summary>
/// 描画開始関数
/// </summary>
void SpriteRenderer::Start()
{
}

/// <summary>
/// 描画終了関数
/// </summary>
void SpriteRenderer::End()
{
}

void SpriteRenderer::DrawGraph(DirectX::SimpleMath::Vector2 pos, ID3D11ShaderResourceView* texture)
{
	DirectX::XMVECTOR color = DirectX::XMVectorSet(1, 1, 1, m_renderState.GetAlpha());
	if (texture) m_spriteBatch->Draw(texture, pos, color);
}
