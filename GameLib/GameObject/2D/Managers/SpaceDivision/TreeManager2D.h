//====================================================//
// ファイル名   : TreeManager2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/08
//
// 概要 : 4分木管理クラス
//
// 更新履歴 :
// 2026/04/08 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <unordered_map>
#include <cstdint>

#include "Cell2D.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class TreeManager2D
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------
    static constexpr unsigned int MANAGER_MAX_LEVEL = 8; // 作成時に指定できる最大のレベル数


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 空間ポインタのリスト
    std::vector<Cell2D*> m_cells;

    // ルート空間のサイズ
    DirectX::SimpleMath::Vector2 m_rootSize;

    // ルート空間の中心座標
    DirectX::SimpleMath::Vector2 m_rootPosition;

    // ルート空間の最大最小
    DirectX::SimpleMath::Vector2 m_rootMin, m_rootMax;

    // 最小領域の空間のサイズ
    DirectX::SimpleMath::Vector2 m_minimumSize;

    // 最大分割数
    unsigned int m_maxLevel;

    // セルの合計数
    unsigned int m_cellCount;

    int m_pow[MANAGER_MAX_LEVEL];    // べき乗計算省略用の配列

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    TreeManager2D(DirectX::SimpleMath::Vector2 size, unsigned int maxLevel, DirectX::SimpleMath::Vector2 center = { 0, 0 })
        : m_cells{}
        , m_rootSize{ size }
        , m_rootPosition{ center }
        , m_rootMin{}
        , m_rootMax{}
        , m_minimumSize{}
        , m_maxLevel{ std::min( MANAGER_MAX_LEVEL - 1, maxLevel ) }
        , m_cellCount{ 0 }
        , m_pow{ 0 }
    {
        // 最大最小を算出
        m_rootMin = m_rootPosition - m_rootSize / 2;
        m_rootMax = m_rootPosition + m_rootSize / 2;

        // べき乗配列を作成
        m_pow[0] = 1;
        for (int i = 1; i < MANAGER_MAX_LEVEL; i++) m_pow[i] = m_pow[i - 1] * 4;

        // 最小空間のサイズを算出
        m_minimumSize = m_rootSize / (std::pow(2, m_maxLevel));

        // セル数を算出
        for (unsigned int i = 0; i <= m_maxLevel; i++) m_cellCount += m_pow[i];

        // 配列用のメモリ確保
        m_cells.resize(m_cellCount);

        std::fill(m_cells.begin(), m_cells.end(), nullptr);
    };
    ~TreeManager2D()
    {
        for (auto& cell : m_cells) {
            delete cell;
        }
        m_cells.clear();
    }

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // オブジェクトを登録する関数
    void AddObject(ObjectForTree2D* obj)
    {
        // AABBを取得
        AABB2D box = obj->m_pObject->GetBoundingBox();

        unsigned int index = GetMortonNumber(box);

        // セルが範囲内
        if (index < m_cellCount)
        {
            // セルがまだ存在しない
            if (!m_cells[index])
            {
                CreateCell(index);
            }

            //オブジェクトの追加
            m_cells[index]->AddObject(obj);
        }

        return;
    }

    // 全ての衝突判定リストを調べる関数
    template <typename TCondition, typename TAction>
    void CheckAllCollisionList(TCondition condition, TAction action)
    {
        // ルート空間の存在をチェック
        if (!m_cells[0])
            return;	// 空間が存在していない

        // ルート空間を処理
        std::vector<BaseCollider2D*> ColStac;
        CheckCollisionList(0, ColStac, condition, action);
    }

    // 衝突判定リストを調べる関数
    template <typename TCondition, typename TAction>
    bool CheckCollisionList(unsigned int Elem, std::vector<BaseCollider2D*>& ColStac, TCondition condition, TAction action)
    {
        // 空間内のオブジェクト同士の衝突リスト作成
        ObjectForTree2D* pObj1 = m_cells[Elem]->GetFirstObject();

        // 1つ目がなくなるまでループ
        while (pObj1 != nullptr)
        {
            ObjectForTree2D* pObj2 = pObj1->m_pPre;
            while (pObj2 != nullptr) 
            {
                BaseCollider2D* a = pObj1->m_pObject; 
                BaseCollider2D* b = pObj2->m_pObject;

                pObj2 = pObj2->m_pPre;

                if (condition(a, b))
                {
                    // 実行
                    action(a, b);
                }
            }
            // 衝突スタックとの衝突リスト作成
            for (auto& col : ColStac) 
            {
                BaseCollider2D* a = pObj1->m_pObject;
                BaseCollider2D* b = col;

                if (condition(a, b))
                {
                    // 実行
                    action(a, b);
                }
            }
            pObj1 = pObj1->m_pPre;
        }

        bool ChildFlag = false;
        // 子空間に移動
        unsigned int ObjNum = 0;
        unsigned int i, NextElem;
        for (i = 0; i < 4; i++) 
        {
            NextElem = Elem * 4 + 1 + i;
            if (NextElem < m_cellCount && m_cells[Elem * 4 + 1 + i]) 
            {
                if (!ChildFlag) 
                {
                    // 登録オブジェクトをスタックに追加
                    pObj1 = m_cells[Elem]->GetFirstObject();
                    while (pObj1) 
                    {
                        ColStac.push_back(pObj1->m_pObject);
                        ObjNum++;
                        pObj1 = pObj1->m_pPre;
                    }
                }
                ChildFlag = true;
                CheckCollisionList(Elem * 4 + 1 + i, ColStac, condition, action);	// 子空間へ
            }
        }

        // ⑤ スタックからオブジェクトを外す
        if (ChildFlag) 
        {
            for (i = 0; i < ObjNum; i++) ColStac.pop_back();
        }

        return true;
    }

    // 空間を作成する関数
    void CreateCell(unsigned int morton)
    {
        // 作成済みの空間にたどり着くまでループ
        while (!m_cells[morton])
        {
            // 新しい空間を作成
            m_cells[morton] = new Cell2D();

            // 親空間へ
            morton = (morton - 1) >> 2;
            if (morton >= m_cellCount) break;
        }
    };

    // ボックスからモートン番号を取得する関数
    unsigned int GetMortonNumber(const AABB2D& box)
    {
        // 2点の最小空間モートン番号を取得
        unsigned int minMorton = GetMinimumIndex(box.min);
        unsigned int maxMorton = GetMinimumIndex(box.max);

        // 排他的論理和をとって所属する空間を調べる
        unsigned int def = minMorton ^ maxMorton;

        unsigned int HiLevel = 1;
        unsigned int i;
        for (i = 0; i < m_maxLevel; i++)
        {
            // 2bitずつずらしながら値が0でないレベルをチェック
            unsigned int Check = (def >> (i * 2)) & 0b11;
            if (Check != 0)
                HiLevel = i + 1;    // レベルを更新
        }
        // 片方の番号から自身の所属するレベルでの番号を取得する
        unsigned int SpaceNum = (maxMorton >> (HiLevel * 2));

        // 自分より上位のレベルの空間数を等比数列を使用し取得
        unsigned int AddNum = (m_pow[m_maxLevel - HiLevel] - 1) / 3;
        SpaceNum += AddNum; // 合計して線形配列でのインデックスを作成

        // セル数より大きき番号の場合
        if (SpaceNum > m_cellCount)
            return 0xffffffff;

        return SpaceNum;
    }

    // 最小空間座標からモートン番号を取得する関数
    unsigned int GetMortonNumber(uint8_t x, uint8_t y)
    {
        return (BitDivision(x)) | (BitDivision(y) << 1);
    }

    // ビットを分割する関数
    unsigned int BitDivision(uint8_t bit)
    {
        unsigned int s = bit;
        s = (s | s << 4) & 0b0000111100001111;
        s = (s | s << 2) & 0b0011001100110011;
        s = (s | s << 1) & 0b0101010101010101;
        return s;
    }

    // 座標から要素番号を求める関数
    unsigned int GetMinimumIndex(const DirectX::SimpleMath::Vector2& point)
    {
        // 各要素を最小空間座標に変換
        uint8_t x = static_cast<uint8_t>((point.x - m_rootMin.x) / m_minimumSize.x);
        uint8_t y = static_cast<uint8_t>((point.y - m_rootMin.y) / m_minimumSize.y);

        // 最小空間座標からモートン番号を取得
        return GetMortonNumber(x, y);
    }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};