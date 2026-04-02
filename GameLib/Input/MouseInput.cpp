#include "pch.h"
#include "MouseInput.h"

DirectX::Mouse::State MouseInput::m_nowMouseState;
DirectX::Mouse::State MouseInput::m_oldMouseState;

MouseInput::MouseInput()
{
	m_oldMouseState = m_nowMouseState;
	m_nowMouseState = DirectX::Mouse::Get().GetState();
}

void MouseInput::MouseUpdate()
{
	m_oldMouseState = m_nowMouseState;
	m_nowMouseState = DirectX::Mouse::Get().GetState();
}

bool MouseInput::GetMouseDown(int MouseCode)
{
	switch (MouseCode)
	{
	case MOUSE_LEFT:
		return !m_oldMouseState.leftButton && m_nowMouseState.leftButton;
		break;
	case MOUSE_MIDDLE:
		return !m_oldMouseState.middleButton && m_nowMouseState.middleButton;
		break;
	case MOUSE_RIGHT:
		return !m_oldMouseState.rightButton && m_nowMouseState.rightButton;
		break;
	default:
		break;
	}

	return false;
}

bool MouseInput::GetMouse(int MouseCode)
{
	switch (MouseCode)
	{
	case MOUSE_LEFT:
		return m_nowMouseState.leftButton;
		break;
	case MOUSE_MIDDLE:
		return m_nowMouseState.middleButton;
		break;
	case MOUSE_RIGHT:
		return m_nowMouseState.rightButton;
		break;
	default:
		break;
	}

	return false;
}

bool MouseInput::GetMouseUp(int MouseCode)
{
	switch (MouseCode)
	{
	case MOUSE_LEFT:
		return m_oldMouseState.leftButton && !m_nowMouseState.leftButton;
		break;
	case MOUSE_MIDDLE:
		return m_oldMouseState.middleButton && !m_nowMouseState.middleButton;
		break;
	case MOUSE_RIGHT:
		return m_oldMouseState.rightButton && !m_nowMouseState.rightButton;
		break;
	default:
		break;
	}

	return false;
}

DirectX::SimpleMath::Vector2 MouseInput::GetMousePoint()
{
	int x = m_nowMouseState.x;
	int y = m_nowMouseState.y;

	return { static_cast<float>(x), static_cast<float>(y) };
}

DirectX::SimpleMath::Vector2 MouseInput::GetOldMousePoint()
{
	int x = m_oldMouseState.x;
	int y = m_oldMouseState.y;

	return { static_cast<float>(x), static_cast<float>(y) };
}

DirectX::SimpleMath::Vector2 MouseInput::GetMouseMoveValue()
{
	if (m_nowMouseState.positionMode == DirectX::Mouse::Mode::MODE_ABSOLUTE) 
	{
		GetMousePoint() - GetOldMousePoint();
	}
	if (m_nowMouseState.positionMode == DirectX::Mouse::Mode::MODE_RELATIVE) 
	{
		int x = m_nowMouseState.x;
		int y = m_nowMouseState.y;

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	return { 0, 0 };
}

int MouseInput::GetWheelMoveValue()
{
	return m_nowMouseState.scrollWheelValue - m_oldMouseState.scrollWheelValue;
}

void MouseInput::SetMode(DirectX::Mouse::Mode mode)
{
	DirectX::Mouse::Get().SetMode(mode);
}
