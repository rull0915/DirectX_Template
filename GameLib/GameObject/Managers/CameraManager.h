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

private:
    CameraManager();
    ~CameraManager();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
public:

    // シングルトン化
    static CameraManager& Instance()
    {
        static CameraManager instance;
        return instance;
    }

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
    const DirectX::SimpleMath::Matrix& GetView() { return m_mainCamera->GetView(); }
    const DirectX::SimpleMath::Matrix& GetProj() { return m_mainCamera->GetProj(); }
    const DirectX::SimpleMath::Matrix& GetInverseView() { return m_mainCamera->GetInverseView(); }
    const DirectX::SimpleMath::Matrix& GetInverseProj() { return m_mainCamera->GetInverseProj(); }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};