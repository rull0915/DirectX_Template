//====================================================//
// ファイル名  : RendererComponentManager.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/05/03
//
// 概要        :  描画コンポーネントを管理するクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "RendererComponentManager.h"

//====================================================//
// 関数の実体宣言
//====================================================//

RendererComponentManager::RendererComponentManager()
	: m_reserves{}
	, m_removeReserves{}
	, m_renderers{}
{
}

RendererComponentManager::~RendererComponentManager()
{
}

/// <summary>
/// 更新処理
/// </summary>
void RendererComponentManager::Update()
{
	// 予約されているRendererの追加
	AddReserved();

	// 削除予約のRendererを削除
	RemoveReserved();
}

/// <summary>
/// 全ての描画を行う関数
/// </summary>
void RendererComponentManager::DrawAll(Renderer& renderer)
{
	// 管理している全てのコンポーネントの描画処理を呼び出す
	for (auto& component : m_renderers)
	{
		component->Draw(renderer);
	}
}
