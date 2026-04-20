//====================================================//
// ファイル名  : CameraComponent.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要        :  カメラコンポーネント
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "CameraComponent.h"

#include "GameLib/GameObject/3D/GameObject.h"

#include "../../Managers/CameraManager.h"

//====================================================//
// 関数の実体宣言
//====================================================//

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="own">所持者のポインタ</param>
/// <param name="width">画面幅</param>
/// <param name="height">画面高さ</param>
/// <param name="fov">画角</param>
/// <param name="nearZ">ニアクリップ</param>
/// <param name="farZ">ファークリップ</param>
CameraComponent::CameraComponent(GameObject* own, int width, int height, ProjectionType type, float fov, float nearZ, float farZ, float size)
	: BaseComponent(own, TYPE_ID, IS_MAIN)
	, m_type{ type }
	, m_fov{ fov }
	, m_aspect{ static_cast<float>(width) / height }
	, m_nearZ{ nearZ }
	, m_farZ{ farZ }
	, m_size{ size }
	, m_targetPoint{ 0, 0, 0 }
	, m_useTargetPoint{ false }
{
	// 投影行列の作成
	UpdateProj();

	UpdateView();
}

/// <summary>
/// デストラクタ
/// </summary>
CameraComponent::~CameraComponent()
{
}

void CameraComponent::SetMain()
{
	// カメラマネージャーに自身を登録
	CameraManager::Instance().SetMainCamera(this);
}

/// <summary>
/// ビュー行列の更新
/// </summary>
void CameraComponent::UpdateView()
{
	// 注視点を使用する場合
	if (m_useTargetPoint)
	{
		// ---- Upベクトルを算出
		using namespace DirectX::SimpleMath;

		// 位置を取得
		auto trans = m_own->GetComponent<Transform>();
		trans->UpdateCache();
		
		DirectX::SimpleMath::Vector3 cameraPos = trans->GetWorldPosition();

		// 各方向ベクトル
		Vector3 forward = m_targetPoint - cameraPos;
		forward.Normalize();

		Vector3 right = Vector3::UnitY.Cross(forward);
		right.Normalize();

		Vector3 up = forward.Cross(right);
		up.Normalize();

		// 行列の作成
		m_view = DirectX::SimpleMath::Matrix::CreateLookAt(
			cameraPos,
			m_targetPoint,
			up);

		m_inverseView = m_view.Invert();
	}
	else
	{
		// 位置をそのまま行列に
		m_inverseView = m_own->GetComponent<Transform>()->GetWorldMatrix();

		m_view = m_inverseView.Invert();
	}
}

void CameraComponent::UpdateProj()
{
	switch (m_type)
	{
	case ProjectionType::Perspective:

		// 各設定から射影行列を作成
		m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
			m_fov,
			m_aspect,
			m_nearZ,
			m_farZ
		);

		break;
	case ProjectionType::Orthographic:

		// 各種設定から行列を作成
		m_proj = DirectX::SimpleMath::Matrix::CreateOrthographic(
			m_aspect * m_size,
			m_size,
			m_nearZ,
			m_farZ
		);

		break;
	default:
		break;
	}

	// 逆行列を計算
	m_inverseProj = m_proj.Invert();
}
