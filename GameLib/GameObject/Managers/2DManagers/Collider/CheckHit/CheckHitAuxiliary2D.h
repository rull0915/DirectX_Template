//====================================================//
// ファイル名   : CheckHitAuxiliary2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/04
//
// 概要 : 衝突判定で使用する補助関数をまとめた2D用のヘッダです
//
// 更新履歴 :
// 2026/04/24 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "GameLib/GameMath/GameMath.h"
using namespace DirectX;

#define EPSILON 0.0001f

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// 関数宣言
//====================================================//

/// <summary>
/// 線分(p1, p2)上で、点pに最も近い点を探す補助関数
/// </summary>
/// <param name="p1"></param>
/// <param name="p2"></param>
/// <param name="p"></param>
/// <returns></returns>
static SimpleMath::Vector2 ClosestPointOnSegment(const SimpleMath::Vector2& p1, const SimpleMath::Vector2& p2, const DirectX::SimpleMath::Vector2& p)
{
    SimpleMath::Vector2 v = p2 - p1;    // 線分の方向ベクトル
    SimpleMath::Vector2 w = p - p1;     // 点からp1への方向ベクトル

    // 線分の長さの2乗
    float l = v.LengthSquared();
    if (l < 0.0001f) return p1; // 点に近い線分の場合

    // 投影比率 t を内積から計算
    float t = w.Dot(v) / l;

    // 線分の範囲内にクランプ
    t = MyMath::Clamp(t, 0.0f, 1.0f);

    // 線分上の最近接点
    return p1 + v * t;
}

static float ClosedSegmentToSegment(const SimpleMath::Vector2& p1, const SimpleMath::Vector2& p2, const SimpleMath::Vector2& q1, const SimpleMath::Vector2& q2, SimpleMath::Vector2& outP, SimpleMath::Vector2& outQ)
{
    SimpleMath::Vector2 d1 = p2 - p1; // 線分1の方向ベクトル
    SimpleMath::Vector2 d2 = q2 - q1; // 線分2の方向ベクトル
    SimpleMath::Vector2 r = p1 - q1;
    float a = d1.Dot(d1); // 線分1の長さの2乗
    float e = d2.Dot(d2); // 線分2の長さの2乗
    float f = d2.Dot(r);

    float s, t;

    // 両方の線分が点（長さ0）の場合
    if (a <= EPSILON && e <= EPSILON) {
        s = t = 0.0f;
        outP = p1; outQ = q1;
        return (outP - outQ).LengthSquared();
    }

    if (a <= EPSILON) {
        // 線分1が点の場合
        s = 0.0f;
        t = MyMath::Clamp(f / e, 0.0f, 1.0f);
    }
    else {
        float c = d1.Dot(r);
        if (e <= EPSILON) {
            // 線分2が点の場合
            t = 0.0f;
            s = MyMath::Clamp(-c / a, 0.0f, 1.0f);
        }
        else {
            // 一般的なケース
            float b = d1.Dot(d2);
            float den = a * e - b * b;

            // 平行でない場合
            if (den != 0.0f) {
                s = MyMath::Clamp((b * f - c * e) / den, 0.0f, 1.0f);
            }
            else {
                // 平行な場合は任意のs（ここでは0）から始める
                s = 0.0f;
            }

            // sを用いてtを計算し、tが範囲外ならsを再計算する
            t = (b * s + f) / e;

            if (t < 0.0f) {
                t = 0.0f;
                s = MyMath::Clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f) {
                t = 1.0f;
                s = MyMath::Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    outP = p1 + d1 * s;
    outQ = q1 + d2 * t;
    return (outP - outQ).LengthSquared();
}

static SimpleMath::Vector2 ClosedPointOnAABB(const SimpleMath::Vector2& halfSize, const SimpleMath::Vector2 point)
{
    SimpleMath::Vector2 nearPoint = {
    MyMath::Clamp(point.x, -halfSize.x, halfSize.x),
    MyMath::Clamp(point.y, -halfSize.y, halfSize.y)
    };

    return nearPoint;
}
