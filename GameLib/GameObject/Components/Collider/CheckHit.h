//====================================================//
// ファイル名   : CheckHit.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/18
//
// 概要 : コライダーの衝突判定をまとめたヘッダー
//
// 更新履歴 :
// 2026/03/18 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

#include "Shapes/Colliders.h"
#include "BaseCollider.h"

//====================================================//
// 関数宣言
//====================================================//

/// <summary>
/// 衝突をチェックする関数
/// </summary>
/// <param name="col1">1つ目のコライダー</param>
/// <param name="col2">2つ目のコライダー</param>
/// <returns>衝突しているかどうか</returns>
bool CheckHit(BaseCollider* col1, BaseCollider* col2);

/// <summary>
/// 球同士の衝突判定
/// </summary>
bool CheckHit(SphereCollider* col1, SphereCollider* col2);

/// <summary>
/// 球と線の衝突判定
/// </summary>
bool CheckHit(SphereCollider* col1, LineCollider* col2);

/// <summary>
/// 球とAABBの衝突判定
/// </summary>
bool CheckHit(SphereCollider* col1, AABBCollider* col2);

/// <summary>
/// 線とAABBの衝突判定
/// </summary>
bool CheckHit(LineCollider* col1, AABBCollider* col2);

/// <summary>
/// AABB同士の衝突判定
/// </summary>
bool CheckHit(AABBCollider* col1, AABBCollider* col2);


/// <summary>
/// コライダーが持つAABBの衝突判定
/// </summary>
bool CheckAABB(const AABB& aabb1, const AABB& aabb2);
