//====================================================//
// ファイル名   : RenderStateCache.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/29
//
// 概要 : レンダーが使用するステートを管理するクラス
//
// 更新履歴 :
// 2026/04/29 新規作成
//====================================================//

#pragma once

//====================================================//
// 列挙型宣言
//====================================================//

// 描画タイプ
enum RenderType : uint16_t
{
    PRIMITIVE = 0,
    SPRITE,
    FONT,
    RENDER_TYPE_COUNT
};

// 行列タイプ
enum MatrixType : uint16_t
{
    WORLD      = 1 << 0,
    VIEW       = 1 << 1,
    PROJECTION = 1 << 2,
    ALL        = (WORLD | VIEW | PROJECTION)
};

//====================================================//
// クラス宣言
//====================================================//
class RenderStateCache
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 各行列
    DirectX::SimpleMath::Matrix m_world;
    DirectX::SimpleMath::Matrix m_view;
    DirectX::SimpleMath::Matrix m_projection;

    // α値
    float m_alphaValue;

    // 変更フラグ
    // 描画タイプがキー 行列タイプがbit毎に管理されています。
    uint16_t m_isDirty[RenderType::RENDER_TYPE_COUNT];

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    RenderStateCache()
        : m_world{ DirectX::SimpleMath::Matrix::Identity }
        , m_view{ DirectX::SimpleMath::Matrix::Identity }
        , m_projection{ DirectX::SimpleMath::Matrix::Identity }
        , m_alphaValue{ 1.0f }
        , m_isDirty{ 0 }
    {
        for (auto& flags : m_isDirty)
        {
            flags |= MatrixType::ALL;
        }
    };
    ~RenderStateCache() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    void ResetFlag(RenderType renderId, MatrixType matrixId)
    {
        m_isDirty[renderId] &= ~matrixId;
    }
    bool IsDirty(RenderType renderId, MatrixType matrixId) const
    {
        return m_isDirty[renderId] & matrixId;
    }

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    const DirectX::SimpleMath::Matrix& GetWorld() const { return m_world; }
    const DirectX::SimpleMath::Matrix& GetView() const { return m_view; }
    const DirectX::SimpleMath::Matrix& GetProjection() const { return m_projection; }
    float GetAlpha() const { return m_alphaValue; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

    void SetWorld(const DirectX::SimpleMath::Matrix& world) 
    {
        if (world != m_world)
        {
            for (auto& flags : m_isDirty)
            {
                flags |= MatrixType::WORLD;
            }
            m_world = world;
        }
    }
    void SetView(const DirectX::SimpleMath::Matrix& view)
    {
        if (view != m_view)
        {
            for (auto& flags : m_isDirty)
            {
                flags |= MatrixType::VIEW;
            }            m_view = view;
        }
    }
    void SetProjection(const DirectX::SimpleMath::Matrix& projection)
    {
        if (projection != m_projection)
        {
            for (auto& flags : m_isDirty)
            {
                flags |= MatrixType::PROJECTION;
            }            
            m_projection = projection;
        }
    }
    void SetAlpha(float alpha) { m_alphaValue = alpha; }
};