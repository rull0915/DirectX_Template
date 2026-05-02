//====================================================//
// ファイル名  : Renderer.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/29
//
// 概要        :  描画を統括するクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "Renderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
Renderer::Renderer()
	: m_device{ nullptr }
	, m_nowContext{ nullptr }
	, m_states{ nullptr }
	, m_drawState{}
	, m_proxy{ this, SwitchRendererProxy }
	, m_primitiveRenderer{}
	, m_modelRenderer{}
	, m_isStarted{ false }
	, m_isInitialized{ false }
{
}

/// <summary>
/// 初期化関数
/// </summary>
void Renderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::CommonStates* state)
{
	if (m_isInitialized) return;

	// メンバの初期化
	m_device = device;

	m_nowContext = context;

	m_states = state;

	// 各レンダーの初期化

	m_primitiveRenderer = std::make_unique<PrimitiveRenderer>(*this);
	m_modelRenderer = std::make_unique<ModelRenderer>(*this);

	m_primitiveRenderer->Initialize();
	m_modelRenderer->Initialize();

	// ProxyにRendererを渡す
	m_proxy.SetPrimitive(m_primitiveRenderer.get());
	m_proxy.SetModel(m_modelRenderer.get());

	// フラグのセット
	m_isInitialized = true;

	// レンダラーの初期化

	m_nowRenderer = nullptr;
	m_nowType = RendererType::None;
}

void Renderer::Start(ID3D11DeviceContext* context)
{
	if (m_isStarted) return;

	// コンテキストの更新
	m_nowContext = context;

	// 実行中レンダラーをリセット
	m_nowRenderer = nullptr;
	m_nowType = RendererType::None;

	// フラグのセット
	m_isStarted = true;
}

void Renderer::End()
{
	if (!m_isStarted) return;

	// 実行中のレンダラーの終了処理
	if (m_nowRenderer) m_nowRenderer->End();

	// 実行中レンダラーをリセット
	m_nowRenderer = nullptr;
	m_nowType = RendererType::None;

	m_isStarted = false;
}

RenderProxy& Renderer::Draw()
{
	// 初期化チェック
	assert(m_isInitialized);

	// 描画開始チェック
	assert(m_isStarted);

	// 仲介クラスを返す
	return m_proxy;
}
