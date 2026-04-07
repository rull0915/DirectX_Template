//====================================================//
// ファイル名   : CheckHitAuxiliary.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/04
//
// 概要 : 衝突判定で使用する補助関数をまとめたヘッダです
//
// 更新履歴 :
// 2026/04/04 新規作成
//====================================================//

#pragma once
#define Vector3 DirectX::SimpleMath::Vector3

//====================================================//
// インクルードファイル
//====================================================//


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
static Vector3 ClosestPointOnSegment(const Vector3& p1, const Vector3& p2, const Vector3& p)
{
    Vector3 v = p2 - p1;    // 線分の方向ベクトル
    Vector3 w = p - p1;     // 点からp1への方向ベクトル

    // 線分の長さの2乗
    float l2 = v.LengthSquared();
    if (l2 < 0.0001f) return p1; // 点に近い線分の場合

    // 投影比率 t を内積から計算
    float t = w.Dot(v) / l2;

    // 線分の範囲内にクランプ
    t = std::max(0.0f, std::min(1.0f, t));

    // 線分上の最近接点
    return p1 + v * t;
}

/// <summary>
/// 線分同士の最短距離を求める関数
/// </summary>
static float ClosedSegmentToSegment(const Vector3& p1, const Vector3& p2, const Vector3& q1, const Vector3& q2, Vector3& outP, Vector3& outQ)
{
    // 線分のベクトル
    Vector3 vP = p2 - p1, vQ = q2 - q1;
    Vector3 PtoQ = p1 - q1;

    // 線分の長さの2乗を求める
    float sqLP = vP.Dot(vP), sqLQ = vQ.Dot(vQ);

    // 2直線の外積
    float dt = vP.Dot(vQ);
    float den = sqLP * sqLQ - dt * dt; // 行列式

    // 線分が短い場合
    if (sqLP < 0.001f)
    {
        outP = p1, outQ = ClosestPointOnSegment(q1, q2, p1);
        return (outP - outQ).LengthSquared();
    }
    if (sqLQ < 0.001f)
    {
        outP = ClosestPointOnSegment(p1, p2, q1), outQ = q1;
        return (outP - outQ).LengthSquared();
    }

    // 平行の場合
    if (den < 0.001f)
    {
        // p1が最近点だと仮定
        // 投影比率 t を内積から計算
        float tP = 0, tQ = PtoQ.Dot(vQ) / sqLQ;

        // 線分内の場合
        if (tQ >= 0 && tQ <= 1)
        {
            outP = p1 + tP * vP; outQ = q1 + tQ * vQ;
            return (outP - outQ).LengthSquared();
        }
        // さらに線分外なら
        tQ = MyMath::Clamp(tQ, 0, 1);   // Qを0 ~ 1にクランプ

        outP = p1 + tP * vP; outQ = q1 + tQ * vQ;
        return (outP - outQ).LengthSquared();
    }
    // 平行じゃない場合
    else
    {
        // 2直線の最近点2つを求める
        float tP = (dt * vQ.Dot(PtoQ) - sqLQ * vP.Dot(PtoQ)) / den;
        float tQ = (vQ.Dot(PtoQ + tP * vP)) / sqLQ;

        // 2点の状況で場合分け
        // どちらも線分内なら
        if ((tP >= 0 && tP <= 1) && (tQ >= 0 && tQ <= 1))
        {
            outP = p1 + tP * vP; outQ = q1 + tQ * vQ;
            return (outP - outQ).LengthSquared();
        }
        // どちらかが線分外なら
        tP = MyMath::Clamp(tP, 0, 1);   // Pを0 ~ 1にクランプ

        // クランプ後の点から最近点を求める
        Vector3 pP = p1 + tP * vP;
        Vector3 w = pP - q1;     // 点からp1への方向ベクトル

        // 投影比率 t を内積から計算
        tQ = w.Dot(vQ) / sqLQ;

        // 線分内の場合
        if (tQ >= 0 && tQ <= 1)
        {
            outP = p1 + tP * vP; outQ = q1 + tQ * vQ;
            return (outP - outQ).LengthSquared();
        }
        // さらに線分外なら
        tQ = MyMath::Clamp(tQ, 0, 1);   // Qを0 ~ 1にクランプ

        outP = p1 + tP * vP; outQ = q1 + tQ * vQ;
        return (outP - outQ).LengthSquared();
    }
}

inline static float LengthSegmentOnSeparateAxis(const Vector3& target, const Vector3& x, const Vector3& y, const Vector3& z)
{
    return abs(target.Dot(x)) + abs(target.Dot(y)) + abs(target.Dot(z));
}

static bool CheckArea(int idX, int idY, int idZ, Vector3 start, Vector3 end, float tMin, float tMax, Vector3 size, float radius, Vector3& outSeg, Vector3& outBox)
{
    if (idX < 0 || idX > 2 || idY < 0 || idY > 2 || idZ < 0 || idZ > 2) return false;

    // 中心の場合
    if (idX == 1 && (idY == 1 && idZ == 1))
    {
        // 線分の中点
        Vector3 center = (start + end) / 2;

        // X軸の判定：右の壁か左の壁か
        Vector3 distToWall, sign;

        distToWall.x = size.x - abs(center.x); // 壁までの最短距離
        sign.x = (center.x >= 0) ? 1.0f : -1.0f; // どちら側の壁か（法線の向き）

        distToWall.y = size.y - abs(center.y);
        sign.y = (center.y >= 0) ? 1.0f : -1.0f;

        distToWall.z = size.z - abs(center.z);
        sign.z = (center.z >= 0) ? 1.0f : -1.0f;

        // 3つの軸の中で、最も距離が短い（＝脱出しやすい）軸を探す
        float minDist = distToWall.x;
        Vector3 localNormal = { sign.x, 0, 0 };

        if (distToWall.y < minDist) {
            minDist = distToWall.y;
            localNormal = { 0, sign.y, 0 };
        }
        if (distToWall.z < minDist) {
            minDist = distToWall.z;
            localNormal = { 0, 0, sign.z };
        }

        // 最終的な押し戻しベクトル
        outSeg = center;
        outBox = center + localNormal * (minDist * radius);

        return true;
    }

    // --- 面の場合 ---
    // (idX, idY, idZ のうち 2つが 1 である状態)
    int centers = (idX == 1) + (idY == 1) + (idZ == 1);
    if (centers == 2)
    {
        Vector3 faceNormal(0, 0, 0);
        float wallPos = 0;

        // どの軸の面かを特定
        if (idX != 1) {
            faceNormal.x = (idX == 0) ? -1.0f : 1.0f;
            wallPos = size.x * faceNormal.x;
        }
        else if (idY != 1) {
            faceNormal.y = (idY == 0) ? -1.0f : 1.0f;
            wallPos = size.y * faceNormal.y;
        }
        else if (idZ != 1) {
            faceNormal.z = (idZ == 0) ? -1.0f : 1.0f;
            wallPos = size.z * faceNormal.z;
        }

        // 線分を面の範囲（size）でクランプして、面上の最近点を探す
        // 例：X面なら、startとendのY, Zをボックスの範囲内に収める
        Vector3 p1 = start + (end - start) * tMin;
        Vector3 p2 = start + (end - start) * tMax;

        // p1, p2 のうち、より「壁の外側」に近い方、あるいはめり込んでいる方を選ぶ
        // 面の法線方向の距離を比較
        float d1 = 0, d2 = 0;
        if (idX != 1) {
            d1 = abs(p1.x - wallPos);
            d2 = abs(p2.x - wallPos);
        }
        else if (idY != 1) {
            d1 = abs(p1.y - wallPos);
            d2 = abs(p2.y - wallPos);
        }
        else if (idZ != 1) {
            d1 = abs(p1.z - wallPos);
            d2 = abs(p2.z - wallPos);
        }

        // より壁に近い（または外に出ている）点を選択
        outSeg = (d1 < d2) ? p1 : p2;

        // 面上の最近点（outSegをその平面に投影したもの）
        outBox = outSeg;
        if (idX != 1) outBox.x = wallPos;
        else if (idY != 1) outBox.y = wallPos;
        else if (idZ != 1) outBox.z = wallPos;

        return true;
    }

    // 辺の場合
    if (idX == 1 || idY == 1 || idZ == 1)
    {
        // 辺の2点を決定
        Vector3 aStart, aEnd;
        switch (idX)
        {
        case 0: aStart.x = -size.x, aEnd.x = -size.x; break;
        case 1: aStart.x = -size.x, aEnd.x = size.x; break;
        case 2: aStart.x = size.x, aEnd.x = size.x; break;
        }
        switch (idY)
        {
        case 0: aStart.y = -size.y, aEnd.y = -size.y; break;
        case 1: aStart.y = -size.y, aEnd.y = size.y; break;
        case 2: aStart.y = size.y, aEnd.y = size.y; break;
        }
        switch (idZ)
        {
        case 0: aStart.z = -size.z, aEnd.z = -size.z; break;
        case 1: aStart.z = -size.z, aEnd.z = size.z; break;
        case 2: aStart.z = size.z, aEnd.z = size.z; break;
        }

        Vector3 outA, outB;

        // 線分同士の最短距離が半径以下かどうかを返す
        float lenSq = ClosedSegmentToSegment(start, end, aStart, aEnd, outA, outB);
        outSeg = outA, outBox = outB;

        return lenSq <= radius * radius;
    }

    // 点の場合
    
    // 点の座標を決定
    Vector3 point;
    
    switch (idX)
    {
    case 0: point.x = -size.x; break;
    case 2: point.x = size.x; break;
    default: point.x = 0;      break;
    }
    switch (idY)
    {
    case 0: point.y = -size.y; break;
    case 2: point.y = size.y; break;
    default: point.y = 0;      break;
    }
    switch (idZ)
    {
    case 0: point.z = -size.z; break;
    case 2: point.z = size.z; break;
    default: point.z = 0;      break;
    }

    Vector3 nearP = ClosestPointOnSegment(start, end, point);
    outSeg = nearP, outBox = point;

    return (nearP - point).LengthSquared() <= radius * radius;
}
