#include "pch.h"
#include "Button.h"

#include "GameLib/MyRenderer.h"

#include "GameLib/Input/MouseInput.h"

Button::Button(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, int stColor, int hvColor, int prColor)
	: m_position{ pos }
	, m_size{ size }
	, m_standardColor{ stColor }
	, m_hoveredColor{ hvColor }
	, m_pressedColor{ prColor }
	, m_nowMode{ Mode::Normal }
	, m_pressDown{ false }
	, m_press{ false }
	, m_pressUp{ false }
	, m_isActive{ true }
{
}

Button::~Button()
{
}

void Button::Initialize()
{
	m_press = false;
	m_pressDown = false;
	m_pressUp = false;

	m_nowMode = Mode::Normal;
}

void Button::Update()
{
	if (!m_isActive) return;

	// マウスの位置を取得
	DirectX::SimpleMath::Vector2 mousePos = MouseInput::GetMousePoint();

	m_pressDown = false;
	m_press = false;
	m_pressUp = false;

	switch (m_nowMode)
	{
	case Button::Mode::Normal:

		// マウスがボタン上にあれば
		if (PointOnMe(mousePos)) m_nowMode = Button::Mode::Hover;

		break;
	case Button::Mode::Hover:

		// マウスがボタンから離れたら
		if (!PointOnMe(mousePos)) m_nowMode = Button::Mode::Normal;

		// クリックされたら
		if (MouseInput::GetMouseDown(MOUSE_LEFT))
		{
			m_nowMode = Button::Mode::Press;

			m_pressDown = true;
			m_press = true;
		}

		break;
	case Button::Mode::Press:

		m_pressDown = false;
		m_press = true;

		// 離されたら
		if (MouseInput::GetMouseUp(MOUSE_LEFT))
		{
			// マウスがボタン上にあれば
			if (PointOnMe(mousePos))
			{
				m_pressUp = true;
			}

			m_press = false;

			m_nowMode = Mode::Normal;
		}

		break;
	default:
		break;
	}
}

void Button::Render() const
{
	if (!m_isActive) return;

	int color = 0;

	switch (m_nowMode)
	{
	case Button::Mode::Normal:
		color = m_standardColor;
		break;
	case Button::Mode::Hover:
		color = m_hoveredColor;
		break;
	case Button::Mode::Press:
		color = m_pressedColor;
		break;
	default:
		break;
	}

	MyRenderer::DrawBox(m_position - m_size / 2, m_position + m_size / 2, color);
}

bool Button::PointOnMe(DirectX::SimpleMath::Vector2 point) const
{
	if (point.x < m_position.x - m_size.x / 2) return false;
	if (point.x > m_position.x + m_size.x / 2) return false;
	if (point.y < m_position.y - m_size.y / 2) return false;
	if (point.y > m_position.y + m_size.y / 2) return false;

	return true;
}
