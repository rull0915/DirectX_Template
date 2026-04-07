//====================================================//
// ファイル名   : Camera.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/02
//
// 概要 : 基本的なカメラクラスです
//
// 更新履歴 :
// 2026/03/02 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../GameObject.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class Camera : public GameObject
{
private:

	//==============================
	// matrix
	//==============================

	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_inverseView;
	DirectX::SimpleMath::Matrix m_proj;
	DirectX::SimpleMath::Matrix m_inverseProj;

	//==============================
	// projection param
	//==============================

	float m_fov;
	float m_aspect;
	float m_nearZ;
	float m_farZ;

	//==============================
	// pthers
	//==============================
	// 注視点の位置
	DirectX::SimpleMath::Vector3 m_targetPoint;

	// 注視点を使用するかどうか
	bool m_useTargetPoint;

public:
	Camera(float width, float height);

	void Update();

private:

	// 行列の更新
	void UpdateView();
	void UpdateProj();

	// ======== セッター ======== //
public:

	void SetTargetPosition(DirectX::SimpleMath::Vector3 pos){ m_targetPoint = pos;}

	void SetUseTarget(bool active){ m_useTargetPoint = active;}

	// ======== ゲッター ======== //
public:

	DirectX::SimpleMath::Matrix& GetView() { return m_view; }
	DirectX::SimpleMath::Matrix& GetProj() { return m_proj; }

	DirectX::SimpleMath::Matrix& GetInverseView() { return m_inverseView; }
	DirectX::SimpleMath::Matrix& GetInverseProj() { return m_inverseProj; }
};
