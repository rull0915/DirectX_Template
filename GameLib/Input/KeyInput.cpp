#include "pch.h"
#include "KeyInput.h"

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
