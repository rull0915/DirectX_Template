//====================================================//
// ファイル名   : HitInfomation.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/02
//
// 概要 : 衝突情報構造体です
//
// 更新履歴 :
// 2026/04/02 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../../../GameObject.h"

//====================================================//
// 構造体宣言
//====================================================//
struct HitInfomation
{
    DirectX::SimpleMath::Vector3 hitDir;      // 押し戻し方向（相手から自分へ向かうベクトル）
    float hitLen;        // 押し戻し距離（めり込み深さ）
    GameObject* own;     // 自分自身
    GameObject* target;  // 衝突相手

    // 情報を完全に反転させたものを返す
    HitInfomation Inverse() const {
        HitInfomation inv;
        inv.hitDir = -this->hitDir; // 方向を反転
        inv.hitLen = this->hitLen;  // 距離は不変
        inv.own = this->target;     // 自分のターゲットを、新しい「自分」に
        inv.target = this->own;     // 自分自身を、新しい「ターゲット」に
        return inv;
    }

    HitInfomation()
        : hitDir{ 0, 0, 0 }
        , hitLen{ 0.0f }
        , own{ nullptr }
        , target{ nullptr }
    {
    }
};