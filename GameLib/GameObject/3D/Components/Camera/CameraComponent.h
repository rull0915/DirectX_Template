//====================================================//
// ファイル名   : CameraComponent.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要 : カメラをコンポーネント化したもの
//
// 更新履歴 :
// 2026/04/16 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../BaseComponent.h"

//====================================================//
// 前方宣言
//====================================================//

enum class ProjectionType
{
    Perspective,
    Orthographic,
};

//====================================================//
// クラス宣言
//====================================================//
class CameraComponent : public BaseComponent
{
public:
    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------
    static constexpr int TYPE_ID = MAIN_CAMERA;
    static constexpr bool IS_MAIN = true;

private:
    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 投影モード
    ProjectionType m_type;

    // --- 射影行列の情報 --- //

    // 共通
    float m_nearZ;  // ニアクリップ
    float m_farZ;   // ファークリップ
    float m_aspect; // アスペクト比

    // Perspective
    float m_fov;    // 画角

    // Orthographic
    float m_size;
     
    // --- 各種行列 --- //
    DirectX::SimpleMath::Matrix m_view; // ビュー
    DirectX::SimpleMath::Matrix m_proj; // 射影
    DirectX::SimpleMath::Matrix m_inverseView;  // ビュー逆行列
    DirectX::SimpleMath::Matrix m_inverseProj;  // 射影逆行列

    // --- その他変数 --- //

    DirectX::SimpleMath::Vector3 m_targetPoint; // 注視点の位置
    bool m_useTargetPoint;    // 注視点を使用するかどうか

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    CameraComponent(GameObject* own, int width, int height, 
        ProjectionType type = ProjectionType::Perspective, 
        float fov = 45.0f, float nearZ = 0.1f, float farZ = 100.0f,
        float size = 30.0f);
    ~CameraComponent();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void SetMain();

    const DirectX::SimpleMath::Matrix& GetView() { return m_view; }
    const DirectX::SimpleMath::Matrix& GetProj() { return m_proj; }
    const DirectX::SimpleMath::Matrix& GetInverseView() { return m_inverseView; }
    const DirectX::SimpleMath::Matrix& GetInverseProj() { return m_inverseProj; }

    // タイプを変更する関数
    void SetType(ProjectionType type)
    {
        m_type = type;
        UpdateProj();
    }

    // タイプを変更する関数
    void ChangeType()
    {
        switch (m_type)
        {
        case ProjectionType::Perspective:
            m_type = ProjectionType::Orthographic;
            break;
        case ProjectionType::Orthographic:
            m_type = ProjectionType::Perspective;
            break;
        default:
            break;
        }
        UpdateProj();
    }

    // 各行列を更新する関数
    void UpdateView();
    void UpdateProj();
};