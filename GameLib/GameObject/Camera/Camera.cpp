#include "Camera.h"

using namespace DirectX;

Camera::Camera(float width, float height)
	: m_useTargetPoint{ false }
	, m_targetPoint{ 0, 0, 0 }
{
	m_cameraPosition = { 0,0,0 };
	m_cameraRotation = { XMConvertToRadians(0.0f),XMConvertToRadians(0.0f),0 };
	m_cameraScale = 1;

	m_fov = DirectX::XMConvertToRadians(60.0f);
	m_aspect = width / height;
	m_nearZ = 0.1f;
	m_farZ = 100.0f;

	UpdateView();
	UpdateProj();
}

void Camera::Update()
{
	// カメラの移動
//	Move();

//	Rotate();

	UpdateView();
}

void Camera::Move()
{
	SimpleMath::Matrix rot =
		SimpleMath::Matrix::CreateFromYawPitchRoll(
			m_cameraRotation.y,
			m_cameraRotation.x,
			m_cameraRotation.z);

	SimpleMath::Vector3 forward = SimpleMath::Vector3::TransformNormal(
		SimpleMath::Vector3(0, 0, -1), rot);
	SimpleMath::Vector3 right = SimpleMath::Vector3::TransformNormal(
		SimpleMath::Vector3(1, 0, 0), rot);

	// カメラの移動
	if (KeyInput::GetKey(Keyboard::Keys::W))
	{
		SimpleMath::Vector3 tForward = { forward.x, 0, forward.z };
		tForward.Normalize();

		m_cameraPosition += tForward * 0.1;
	}
	if (KeyInput::GetKey(Keyboard::Keys::S))
	{
		SimpleMath::Vector3 tForward = { forward.x, 0, forward.z };
		tForward.Normalize();

		m_cameraPosition -= tForward * 0.1;
	}
	if (KeyInput::GetKey(Keyboard::Keys::D))
	{
		SimpleMath::Vector3 tRight = { right.x, 0, right.z };
		tRight.Normalize();

		m_cameraPosition += tRight * 0.1;
	}
	if (KeyInput::GetKey(Keyboard::Keys::A))
	{
		SimpleMath::Vector3 tRight = { right.x, 0, right.z };
		tRight.Normalize();

		m_cameraPosition -= tRight * 0.1;
	}
	if (KeyInput::GetKey(Keyboard::Keys::Space))
	{
		m_cameraPosition.y += 0.1f;
	}
	if (KeyInput::GetKey(Keyboard::Keys::LeftShift))
	{
		m_cameraPosition.y -= 0.1f;
	}
}

// カメラの回転
void Camera::Rotate()
{
	SimpleMath::Vector2 mouseMove = MouseInput::GetMouseMoveValue();

	m_cameraRotation.y -= XMConvertToRadians(mouseMove.x * 0.1f);
	m_cameraRotation.x -= XMConvertToRadians(mouseMove.y * 0.1f);
}

void Camera::UpdateView()
{
	// 注視点を使用する場合
	if (m_useTargetPoint)
	{
		// ---- Upベクトルを算出
		using namespace DirectX::SimpleMath;

		Vector3 forward = m_targetPoint - m_cameraPosition;
		forward.Normalize();

		Vector3 right = Vector3::UnitY.Cross(forward);
		right.Normalize();

		Vector3 up = forward.Cross(right);
		up.Normalize();

		m_view = SimpleMath::Matrix::CreateLookAt(
			m_cameraPosition, 
			m_targetPoint, 
			Vector3::Up);

		m_inverseView = m_view.Invert();
	}
	else
	{
		m_inverseView =
			SimpleMath::Matrix::CreateScale(m_cameraScale) *
			SimpleMath::Matrix::CreateFromYawPitchRoll(
				m_cameraRotation.y,
				m_cameraRotation.x,
				m_cameraRotation.z) *
			SimpleMath::Matrix::CreateTranslation(m_cameraPosition);

		m_view = m_inverseView.Invert();
	}
}

void Camera::UpdateProj()
{
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		m_fov,
		m_aspect,
		m_nearZ,
		m_farZ
	);

	m_inverseProj = m_proj.Invert();
}

