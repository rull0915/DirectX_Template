//====================================================//
// ファイル名   : TreeManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/08
//
// 概要 : 分木管理クラス
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

#include "Cell.h"

#include "GameLib/MyRenderer.h" // テスト用

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class TreeManager
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
    std::vector<Cell*> m_cells;

    // ルート空間のサイズ
    DirectX::SimpleMath::Vector3 m_rootSize;

    // ルート空間の中心座標
    DirectX::SimpleMath::Vector3 m_rootPosition;

    // ルート空間の最大最小
    DirectX::SimpleMath::Vector3 m_rootMin, m_rootMax;

    // 最小領域の空間のサイズ
    DirectX::SimpleMath::Vector3 m_minimumSize;

    // 最大分割数
    unsigned int m_maxLevel;

    // セルの合計数
    unsigned int m_cellCount;

    int m_pow[MANAGER_MAX_LEVEL];    // べき乗計算省略用の配列

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    TreeManager(DirectX::SimpleMath::Vector3 size, unsigned int maxLevel, DirectX::SimpleMath::Vector3 center = { 0, 0, 0 })
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
        for (int i = 1; i < MANAGER_MAX_LEVEL; i++) m_pow[i] = m_pow[i - 1] * 8;

        // 最小空間のサイズを算出
        m_minimumSize = m_rootSize / (std::pow(2, m_maxLevel));

        // セル数を算出
        for (int i = 0; i <= m_maxLevel; i++) m_cellCount += m_pow[i];

        // 配列用のメモリ確保
        m_cells.resize(m_cellCount);

        std::fill(m_cells.begin(), m_cells.end(), nullptr);
    };
    ~TreeManager()
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
    void AddObject(ObjectForTree* obj)
    {
        // AABBを取得
        AABB box = obj->m_pObject->GetBoundingBox();

        unsigned int index = GetMortonNumber(box);

        // セルが範囲内
        if (index < m_cellCount)
        {
            // セルがまだ存在しない
            if (!m_cells[index])
            {
                CreateCell(index);
            }

            m_cells[index]->AddObject(obj);
        }

        return;
    }

    // 衝突判定リストを作成する
    template <typename TAction>
    void GetAllCollisionList(TAction action)
    {
        // ルート空間の存在をチェック
        if (!m_cells[0])
            return;	// 空間が存在していない

        int count = 0, hit = 0;

        // ルート空間を処理
        std::vector<BaseCollider*> ColStac;
        GetCollisionList(0, ColStac, action, count, hit);

        count;
        hit;
    }

    // 衝突判定リストを作成する関数
    template <typename TAction>
    bool GetCollisionList(unsigned int Elem, std::vector<BaseCollider*>& ColStac, TAction action, int& count, int& hit)
    {
        // 空間内のオブジェクト同士の衝突リスト作成
        ObjectForTree* pObj1 = m_cells[Elem]->GetFirstObject();
        while (pObj1 != nullptr)
        {
            ObjectForTree* pObj2 = pObj1->m_pPre;
            while (pObj2 != nullptr) {

                count++;

                BaseCollider* a = pObj1->m_pObject; 
                BaseCollider* b = pObj2->m_pObject;

                pObj2 = pObj2->m_pPre;

                // コライダーのアクティブ確認
                if (!a->m_isActive || !b->m_isActive) continue;

                // コライダーの所有者が同じならスキップ
                if (a->m_own == b->m_own) continue;

                // コライダーが持つAABBが衝突していなければスキップ
                AABB aabb1 = a->m_boundingBox, aabb2 = b->m_boundingBox;

                if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) continue;
                if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) continue;
                if (aabb1.min.z > aabb2.max.z || aabb2.min.z > aabb1.max.z) continue;

                hit++;

                // 実行
                action(a, b);
            }
            // 衝突スタックとの衝突リスト作成
            for (auto& col : ColStac) {

                count++;

                BaseCollider* a = pObj1->m_pObject;
                BaseCollider* b = col;

                // コライダーのアクティブ確認
                if (!a->m_isActive || !b->m_isActive) continue;

                // コライダーの所有者が同じならスキップ
                if (a->m_own == b->m_own) continue;

                // コライダーが持つAABBが衝突していなければスキップ
                AABB aabb1 = a->m_boundingBox, aabb2 = b->m_boundingBox;

                if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) continue;
                if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) continue;
                if (aabb1.min.z > aabb2.max.z || aabb2.min.z > aabb1.max.z) continue;

                hit++;

                // 実行
                action(a, b);
            }
            pObj1 = pObj1->m_pPre;
        }

        bool ChildFlag = false;
        // 子空間に移動
        unsigned int ObjNum = 0;
        unsigned int i, NextElem;
        for (i = 0; i < 8; i++) {
            NextElem = Elem * 8 + 1 + i;
            if (NextElem < m_cellCount && m_cells[Elem * 8 + 1 + i]) {
                if (!ChildFlag) {
                    // 登録オブジェクトをスタックに追加
                    pObj1 = m_cells[Elem]->GetFirstObject();
                    while (pObj1) {
                        ColStac.push_back(pObj1->m_pObject);
                        ObjNum++;
                        pObj1 = pObj1->m_pPre;
                    }
                }
                ChildFlag = true;
                GetCollisionList(Elem * 8 + 1 + i, ColStac, action, count, hit);	// 子空間へ
            }
        }

        // ⑤ スタックからオブジェクトを外す
        if (ChildFlag) {
            for (i = 0; i < ObjNum; i++)
                ColStac.pop_back();
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
            m_cells[morton] = new Cell();

            // 親空間へ
            morton = (morton - 1) >> 3;
            if (morton >= m_cellCount) break;
        }
    };

    // ボックスからモートン番号を取得する関数
    unsigned int GetMortonNumber(const AABB& box)
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
            // 3bitずつずらしながら値が0でないレベルをチェック
            unsigned int Check = (def >> (i * 3)) & 0b111;
            if (Check != 0)
                HiLevel = i + 1;    // レベルを更新
        }
        // 片方の番号から自身の所属するレベルでの番号を取得する
        unsigned int SpaceNum = (maxMorton >> (HiLevel * 3));

        // 自分より上位のレベルの空間数を等比数列を使用し取得
        unsigned int AddNum = (m_pow[m_maxLevel - HiLevel] - 1) / 7;
        SpaceNum += AddNum; // 合計して線形配列でのインデックスを作成

        // セル数より大きき番号の場合
        if (SpaceNum > m_cellCount)
            return 0xffffffff;

        return SpaceNum;
    }

    // 最小空間座標からモートン番号を取得する関数
    unsigned int GetMortonNumber(uint8_t x, uint8_t y, uint8_t z)
    {
        return (BitDivision(x)) | (BitDivision(y) << 1) | (BitDivision(z) << 2);
    }

    // ビットを分割する関数
    unsigned int BitDivision(uint8_t bit)
    {
        unsigned int s = bit;
        s = (s | s << 8) & 0b000000001111000000001111;
        s = (s | s << 4) & 0b000011000011000011000011;
        s = (s | s << 2) & 0b001001001001001001001001;
        return s;
    }

    // 座標から要素番号を求める関数
    unsigned int GetMinimumIndex(const DirectX::SimpleMath::Vector3& point)
    {
        // 各要素を最小空間座標に変換
        uint8_t x = static_cast<uint8_t>((point.x - m_rootMin.x) / m_minimumSize.x);
        uint8_t y = static_cast<uint8_t>((point.y - m_rootMin.y) / m_minimumSize.y);
        uint8_t z = static_cast<uint8_t>((point.z - m_rootMin.z) / m_minimumSize.z);

        // 最小空間座標からモートン番号を取得
        return GetMortonNumber(x, y, z);
    }

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};