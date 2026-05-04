//====================================================//
// ファイル名   : RendererManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/05/03
//
// 概要 : 描画コンポーネントを管理するクラス
//
// 更新履歴 :
// 2026/05/03 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../../Components/Renderer/BaseRenderer.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class RendererManager
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


   //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 登録予約中のRenderer
    std::vector<BaseRenderer*> m_reserves;
    std::unordered_set<BaseRenderer*> m_removeReserves;

    // 登録されているBaseRenderer
    std::vector<BaseRenderer*> m_renderers;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    RendererManager();
    ~RendererManager();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // 更新処理
    void Update();

    // 描画処理
    void DrawAll(Renderer& renderer);

    // 登録予約
    void AddRenderer(BaseRenderer* r) { m_reserves.push_back(r); }
    void RemoveRenderer(BaseRenderer* r) { m_removeReserves.insert(r); }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

private:

    // 予約済みポインタの追加
    void AddReserved()
    {
        for (auto p : m_reserves)
        {
            m_renderers.push_back(p);
        }

        m_reserves.clear();
    }

    // 予約済みポインタの削除
    void RemoveReserved()
    {
        for (int i = 0; i < m_renderers.size(); i++)
        {
            // もし削除リストに含まれていたら
            if (m_removeReserves.find(m_renderers[i]) != m_removeReserves.end())
            {
                // 削除
                m_removeReserves.erase(m_renderers[i]);
                m_renderers.erase(m_renderers.begin() + i);

                --i;
            }
        }

        m_removeReserves.clear();
    }
};