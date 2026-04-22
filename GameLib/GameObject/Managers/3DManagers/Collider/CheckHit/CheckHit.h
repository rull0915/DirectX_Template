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

#include "GameLib/GameObject/Components/Collider/3D/Shapes/Colliders.h"
#include "../HitInfomation/HitInfomation.h"

//====================================================//
// 関数宣言
//====================================================//

/// <summary>
/// 衝突をチェックする関数
/// </summary>
/// <param name="col1">1つ目のコライダー</param>
/// <param name="col2">2つ目のコライダー</param>
/// <returns>衝突しているかどうか</returns>
bool CheckHit(BaseCollider* col1, BaseCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// 球同士の衝突判定
/// </summary>
bool CheckHit(SphereCollider* col1, SphereCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// 球とカプセル
/// </summary>
bool CheckHit(SphereCollider* col1, CapsuleCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// 球とボックス
/// </summary>
bool CheckHit(SphereCollider* col1, BoxCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// カプセル同士
/// </summary>
bool CheckHit(CapsuleCollider* col1, CapsuleCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// カプセルとボックス
/// </summary>
bool CheckHit(CapsuleCollider* col1, BoxCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// ボックス同士
/// </summary>
bool CheckHit(BoxCollider* col1, BoxCollider* col2, HitInfomation* info = nullptr);

/// <summary>
/// コライダーが持つAABBの衝突判定
/// </summary>
inline bool CheckAABB(const AABB& aabb1, const AABB& aabb2)
{
	if (aabb1.min.x > aabb2.max.x || aabb2.min.x > aabb1.max.x) return false;
	if (aabb1.min.y > aabb2.max.y || aabb2.min.y > aabb1.max.y) return false;
	if (aabb1.min.z > aabb2.max.z || aabb2.min.z > aabb1.max.z) return false;

	return true;
}

// 線と球
bool CheckHit(LineCollider* col1, SphereCollider* col2);

// 線とカプセル
bool CheckHit(LineCollider* col1, CapsuleCollider* col2);

// 線とボックス
bool CheckHit(LineCollider* col1, BoxCollider* col2);
