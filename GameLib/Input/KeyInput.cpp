//====================================================//
// ファイル名  : KeyInput.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/02/16
//
// 概要        :  キー入力管理クラス
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "KeyInput.h"

//====================================================//
// 関数の実体宣言
//====================================================//

DirectX::Keyboard::State KeyInput::m_nowState;
DirectX::Keyboard::State KeyInput::m_oldState;

KeyInput::KeyInput()
{
	m_nowState = DirectX::Keyboard::Get().GetState();
	m_oldState = m_nowState;
}

void KeyInput::KeyUpdate()
{
	m_oldState = m_nowState;
	m_nowState = DirectX::Keyboard::Get().GetState();
}

bool KeyInput::GetKeyDown(DirectX::Keyboard::Keys key)
{
	return !m_oldState.IsKeyDown(key) && m_nowState.IsKeyDown(key);
}

bool KeyInput::GetKey(DirectX::Keyboard::Keys key)
{
	return 	m_nowState.IsKeyDown(key);
}

bool KeyInput::GetKeyUp(DirectX::Keyboard::Keys key)
{
	return m_oldState.IsKeyDown(key) && !m_nowState.IsKeyDown(key);
}
