//====================================================//
// ファイル名   : Button.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/30
//
// 概要 : ボタンクラス
//
// 更新履歴 :
// 2026/03/30 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//


//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class Button
{
private:

    //-----------------------------------------------------
    // 列挙型
    //-----------------------------------------------------
    enum class Mode
    {
        Normal,
        Hover,
        Press,
    };

    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 中心位置
    DirectX::SimpleMath::Vector2 m_position;

    // サイズ
    DirectX::SimpleMath::Vector2 m_size;

    // 各状態の色
    int m_standardColor;    // 通常時
    int m_hoveredColor;     // カーソルが乗っているとき
    int m_pressedColor;     // 押されているとき

    // 現在の状態
    Mode m_nowMode;

    // 押下状態のフラグ
    bool m_pressDown;
    bool m_press;
    bool m_pressUp;

    // ActiveFrag
    bool m_isActive;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    Button
    (
        DirectX::SimpleMath::Vector2 pos,
        DirectX::SimpleMath::Vector2 size,
        int stColor, int hvColor, int prColor
    );
    ~Button();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
    void Initialize();
    void Update();
    void Render() const;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------

    bool PressDown() const { return m_isActive && m_pressDown; }
    bool Press() const { return m_isActive && m_press; }
    bool PressUp() const { return m_isActive && m_pressUp; }

    DirectX::SimpleMath::Vector2 GetPos() const { return m_position; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetPos(DirectX::SimpleMath::Vector2 pos) { m_position = pos; }

    void SetActive(bool a) { m_isActive = a; }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    // 指定の点が自分に含まれるか
    bool PointOnMe(DirectX::SimpleMath::Vector2 point) const;
};