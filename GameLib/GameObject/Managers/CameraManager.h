//====================================================//
// ファイル名   : CameraManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要 : カメラ管理クラス
//
// 更新履歴 :
// 2026/04/17 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../Components/Camera/CameraComponent.h"
#include "../Components/Camera/RayCast.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class CameraManager
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 使用しているメインカメラ
    CameraComponent* m_mainCamera;

    // レイ
    RayCast m_ray;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
public:
    CameraManager();
    ~CameraManager();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void Update();

    // メインカメラを設定する関数
    void SetMainCamera(CameraComponent* camera)
    {
        m_mainCamera = camera;
    }

    // メインカメラを取得する関数
    CameraComponent* GetMainCamera()
    {
        return m_mainCamera;
    }

    // メインカメラの行列の取得関数
    const DirectX::SimpleMath::Matrix& GetView() { return m_mainCamera ? m_mainCamera->GetView() : DirectX::SimpleMath::Matrix::Identity; }
    const DirectX::SimpleMath::Matrix& GetProj() { return m_mainCamera ? m_mainCamera->GetProj() : DirectX::SimpleMath::Matrix::Identity; }
    const DirectX::SimpleMath::Matrix& GetInverseView() { return m_mainCamera ? m_mainCamera->GetInverseView() : DirectX::SimpleMath::Matrix::Identity; }
    const DirectX::SimpleMath::Matrix& GetInverseProj() { return m_mainCamera ? m_mainCamera->GetInverseProj() : DirectX::SimpleMath::Matrix::Identity; }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};