//====================================================//
// ファイル名  : RendererManager.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/05/03
//
// 概要        :  描画コンポーネントを管理するクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "RendererManager.h"

//====================================================//
// 関数の実体宣言
//====================================================//

RendererManager::RendererManager()
	: m_reserves{}
	, m_removeReserves{}
	, m_renderers{}
{
}

RendererManager::~RendererManager()
{
}

/// <summary>
/// 更新処理
/// </summary>
void RendererManager::Update()
{
	// 予約されているRendererの追加
	AddReserved();

	// 削除予約のRendererを削除
	RemoveReserved();
}

/// <summary>
/// 全ての描画を行う関数
/// </summary>
void RendererManager::DrawAll(Renderer& renderer)
{
	// 管理している全てのコンポーネントの描画処理を呼び出す
	for (auto& component : m_renderers)
	{
		// アクティブチェック
		if (!component->IsActive()) continue;

		component->Draw(renderer);
	}
}
