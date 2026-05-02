//====================================================//
// ファイル名  : RenderProxy.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/29
//
// 概要        :  各関数の呼び出しを担当するクラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "RenderProxy.h"

#include "Renderer.h"

//====================================================//
// 関数の実体宣言
//====================================================//

RenderProxy::RenderProxy(void* renderer, void(*switchRenderer)(void*, RendererType))
	: m_primitive{ nullptr }
	, m_sprite{ nullptr }
	, m_model{ nullptr }
	, m_renderer{ renderer }
	, m_switchRenderer{ switchRenderer }
{
}
