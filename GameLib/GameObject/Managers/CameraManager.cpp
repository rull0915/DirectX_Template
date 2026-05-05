//====================================================//
// ファイル名  : CameraManager.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要        :  カメラ管理クラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "CameraManager.h"

//====================================================//
// 関数の実体宣言
//====================================================//

CameraManager::CameraManager()
	: m_mainCamera{ nullptr }
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::Update()
{


	if(m_mainCamera) m_mainCamera->UpdateView();

	// レイの更新
	if(m_mainCamera) m_ray.Update(*m_mainCamera);
}
