//====================================================//
// ファイル名   : Range.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/19
//
// 概要 : 範囲構造体です
//
// 更新履歴 :
// 2026/03/19 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//


//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
namespace MyMath
{
	class Range
	{
	private:
		float m_min, m_max;

		bool m_isNull;

	public:
		Range(float min, float max, bool adjust = false)
		{
			m_min = min;
			m_max = max;

			if (adjust)
			{
				m_min = min < max ? min : max;
				m_max = min > max ? min : max;
			}

			m_isNull = m_max <= m_min;
		};
		Range() : Range(-FLT_MAX, FLT_MAX) {};

		inline float Min() const { return m_min; };
		inline float Max() const { return m_max; };

		inline bool IsNull() const { return m_isNull; };

		Range CommonPart(const Range& other) const
		{
			float min = (this->Min() > other.Min() ? this->Min() : other.Min());
			float max = (this->Max() < other.Max() ? this->Max() : other.Max());

			return Range(min, max);
		};
	};


	/// <summary>
	/// 2つの範囲の共通部分を返す関数
	/// </summary>
	static Range CommonPart(const Range& a, const Range& b)
	{
		if (a.IsNull()) return a;
		if (b.IsNull()) return b;

		float min = (a.Min() > b.Min() ? a.Min() : b.Min());
		float max = (a.Max() < b.Max() ? a.Max() : b.Max());

		return Range(min, max);
	}
}
