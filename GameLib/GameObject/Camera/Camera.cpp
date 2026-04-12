#include "Camera.h"

using namespace DirectX;

Camera::Camera(float width, float height)
	: m_useTargetPoint{ false }
	, m_targetPoint{ 0, 0, 0 }
{
	m_fov = DirectX::XMConvertToRadians(60.0f);
	m_aspect = width / height;
	m_nearZ = 0.1f;
	m_farZ = 100.0f;

	UpdateView();
	UpdateProj();
}

void Camera::Update(float elapsedTime)
{
	// ビュー行列の更新
	UpdateView();
}

void Camera::UpdateView()
{
	// 注視点を使用する場合
	if (m_useTargetPoint)
	{
		// ---- Upベクトルを算出
		using namespace DirectX::SimpleMath;

		SimpleMath::Vector3 cameraPos = GetComponent<Transform>()->GetWorldPosition();

		Vector3 forward = m_targetPoint - cameraPos;
		forward.Normalize();

		Vector3 right = Vector3::UnitY.Cross(forward);
		right.Normalize();

		Vector3 up = forward.Cross(right);
		up.Normalize();

		m_view = SimpleMath::Matrix::CreateLookAt(
			cameraPos, 
			m_targetPoint, 
			up);

		m_inverseView = m_view.Invert();
	}
	else
	{
		m_inverseView = GetComponent<Transform>()->GetWorldMatrix();

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

