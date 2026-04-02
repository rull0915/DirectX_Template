//====================================================//
// ファイル名   : GameMath.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/19
//
// 概要 : 数学関連の関数をまとめたヘッダー
//
// 更新履歴 :
// 2026/03/19 新規作成
// 2026/03/19 Clamp関数の追加
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Range.h"

//====================================================//
// 関数宣言
//====================================================//
namespace MyMath
{
	// クランプ関数
	static float Clamp(float target, float min, float max)
	{
		if (target <= min) return min;
		if (target >= max) return max;
		return target;
	}

	// ラープ関数
	template<typename T>
	static T Lerp(T start, T goal, float ratio)
	{
		T dir = goal - start;

		return start + dir * ratio;
	}
}
