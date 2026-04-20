//====================================================//
// ファイル名   : CheckHit2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要 : 2Dコライダーの衝突判定をまとめたヘッダー
//
// 更新履歴 :
// 2026/04/17 新規作成
//====================================================//

#pragma once

using Simplex = std::vector<DirectX::SimpleMath::Vector2>;

//====================================================//
// インクルードファイル
//====================================================//

#include "../Shapes/2DColliders.h"
#include "../HitInfomation/HitInfomation2D.h"

//====================================================//
// 関数宣言
//====================================================//

/// <summary>
/// 衝突をチェックする関数
/// </summary>
/// <param name="col1">1つ目のコライダー</param>
/// <param name="col2">2つ目のコライダー</param>
/// <returns>衝突しているかどうか</returns>
bool CheckHit2D(BaseCollider2D* colA, BaseCollider2D* colB, HitInfomation2D* info = nullptr);

// サポート関数
DirectX::SimpleMath::Vector2 SupportAll(BaseCollider2D* col1, BaseCollider2D* col2, DirectX::SimpleMath::Vector2 dir);

// ボックス内の最遠点
DirectX::SimpleMath::Vector2 SupportFunction(BoxCollider2D* col, DirectX::SimpleMath::Vector2 dir);

// 円内の最遠点
DirectX::SimpleMath::Vector2 SupportFunction(CircleCollider2D* col, DirectX::SimpleMath::Vector2 dir);

// ----- その他補助関数 ----- //

// 3重外積
DirectX::SimpleMath::Vector2 TripleProduct(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b, DirectX::SimpleMath::Vector2 c);

DirectX::SimpleMath::Vector2 GetVerticalVector(DirectX::SimpleMath::Vector2 point);

// 反時計回りかどうかを調べる関数
inline float CrossProduct2D(DirectX::SimpleMath::Vector2 p0, DirectX::SimpleMath::Vector2 p1, DirectX::SimpleMath::Vector2 p2) {
    // ベクトル A = p1 - p0
    // ベクトル B = p2 - p0
    return (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
};

// シンプレックスに対する操作を行う関数
bool HandleSimplex(Simplex& simplex, DirectX::SimpleMath::Vector2& dir);

// EPAを用いてめり込みベクトルと深さを求める関数
void GetHitInfo(BaseCollider2D* colA, BaseCollider2D* colB, Simplex& simplex, HitInfomation2D* info);

// 直線上から点への最接近点を求める関数
DirectX::SimpleMath::Vector2 GetNearPointLineToPoint(DirectX::SimpleMath::Vector2 point, DirectX::SimpleMath::Vector2 start, DirectX::SimpleMath::Vector2 dir);
