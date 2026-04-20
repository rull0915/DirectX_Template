//====================================================//
// ファイル名  : CheckHit2D.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/17
//
// 概要        : 2D衝突判定の関数をまとめたcpp 
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "CheckHit2D.h"

using namespace DirectX::SimpleMath;

//====================================================//
// 関数の実体宣言
//====================================================//

bool CheckHit2D(BaseCollider2D* col1, BaseCollider2D* col2, HitInfomation2D* info)
{
	// GJKアルゴリズムを使用して衝突判定を行う

	// 調べるシンプレックスを用意しておく
	Simplex simplex{};

	// 中心同士を結ぶベクトルを初期方向とする
	DirectX::SimpleMath::Vector2 dir = col2->GetWorldCenterPos() - col1->GetWorldCenterPos();
	dir.Normalize();

	// 1つ目のサポート点をX軸を基準として、取得する
	simplex.push_back(SupportAll(col1, col2, dir));

	// 2つ目のサポート点を1つめの点から原点へ向かうベクトルを基準として取得する
	dir = -simplex[0];
	dir.Normalize();

	DirectX::SimpleMath::Vector2 nPoint;

	while (true)
	{
		nPoint = SupportAll(col1, col2, dir);

		// この時点で跨いでいない場合、原点を含まないことが確定するためFalse
		if (dir.Dot(nPoint) < 0) return false;

		// シンプレックスに追加
		simplex.push_back(nPoint);

		if (HandleSimplex(simplex, dir))
		{
			if (info) GetHitInfo(col1, col2, simplex, info);

			return true;
		}
	}

	return false;
}

/// <summary>
/// ミンコフスキー差図形内でdir方向の最遠点を求める関数
/// </summary>
/// <param name="col1">一つ目の図形</param>
/// <param name="col2">二つ目の図形</param>
/// <param name="dir">調べる方向ベクトル(正規化前提)</param>
/// <returns></returns>
DirectX::SimpleMath::Vector2 SupportAll(BaseCollider2D* col1, BaseCollider2D* col2, DirectX::SimpleMath::Vector2 dir)
{
	// 1つずつのサポート点を求めるラムダ式
	auto SupportOnce = [](BaseCollider2D* col, DirectX::SimpleMath::Vector2 dirOnce)-> DirectX::SimpleMath::Vector2
		{
			switch (col->GetType())
			{
			case ColliderType2D::Box:
				return SupportFunction(static_cast<BoxCollider2D*>(col), dirOnce);
			case ColliderType2D::Circle:
				return SupportFunction(static_cast<CircleCollider2D*>(col), dirOnce);
			default:
				break;
			}
		};

	return SupportOnce(col1, dir) - SupportOnce(col2, -dir);
}

/// <summary>
/// ボックスのサポート関数
/// </summary>
DirectX::SimpleMath::Vector2 SupportFunction(BoxCollider2D* col, DirectX::SimpleMath::Vector2 dir)
{
	// 4点の位置ベクトルを求める

	DirectX::SimpleMath::Vector2 points[4]{};

	// 位置
	DirectX::SimpleMath::Vector2 center = col->GetWorldCenterPos();

	// サイズ
	DirectX::SimpleMath::Vector2 half = col->GetHalfSize();

	// 軸
	DirectX::SimpleMath::Vector2 xAxis = col->GetXAxis(), yAxis = col->GetYAxis();

	// 配列の構築
	points[0] = center + half.x * xAxis + half.y * yAxis;
	points[1] = center - half.x * xAxis + half.y * yAxis;
	points[2] = center - half.x * xAxis - half.y * yAxis;
	points[3] = center + half.x * xAxis - half.y * yAxis;

	// 全ての点に対して内積を調べ、最も離れている点を調べる

	// 保持用変数
	int index = 0;

	// 1つ目の点を最大値としておく
	float maxDot = dir.Dot(points[0]);

	// 2つ目以降の点も調べる
	for (int i = 1; i < 4; i++)
	{
		float dot = dir.Dot(points[i]);

		// 最大値を更新したら
		if (dot > maxDot)
		{
			// インデックスと新しい最大値を保持
			index = i;
			maxDot = dot;
		}
	}

	return points[index];
}

/// <summary>
/// 円のサポート関数
/// </summary>
DirectX::SimpleMath::Vector2 SupportFunction(CircleCollider2D* col, DirectX::SimpleMath::Vector2 dir)
{
	// 情報の取得
	DirectX::SimpleMath::Vector2 center = col->GetWorldCenterPos();
	float radius = col->GetRadius();

	return center + dir * radius;
}

DirectX::SimpleMath::Vector2 TripleProduct(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b, DirectX::SimpleMath::Vector2 c)
{
	return b * (a.Dot(c)) - a * (b.Dot(c));
}

DirectX::SimpleMath::Vector2 GetVerticalVector(DirectX::SimpleMath::Vector2 point)
{
	DirectX::SimpleMath::Vector3 thirdDir;
	DirectX::SimpleMath::Vector3 FToS = { point.x, point.y, 0 };
	DirectX::SimpleMath::Vector3 FToO = (abs(point.y / point.x) < 1 ? DirectX::SimpleMath::Vector3{0, 1, 0} : DirectX::SimpleMath::Vector3{1, 0, 0});

	thirdDir = (FToS.Cross(FToO)).Cross(FToS);

	return { thirdDir.x, thirdDir.y };
}

bool HandleSimplex(Simplex& simplex, DirectX::SimpleMath::Vector2& dir)
{
	switch (simplex.size())
	{
		// 線の場合
	case 2:
	{
		// 調べる方向を更新し、falseを返す
		auto ab = simplex[0] - simplex[1];
		dir = TripleProduct(ab, -simplex[1], ab);
		return false;
	}
		// 三角形の場合
	case 3:
	{
		// 2つの領域に原点が含まれるかを調べる
		auto ab = simplex[1] - simplex[2], ac = simplex[0] - simplex[2], ao = -simplex[2];

		DirectX::SimpleMath::Vector2 ABperp = TripleProduct(ac, ab, ab);
		DirectX::SimpleMath::Vector2 ACperp = TripleProduct(ab, ac, ac);

		// 内積を調べる
		if (-simplex[2].Dot(ABperp) > 0)
		{
			simplex.erase(simplex.begin());
			dir = ABperp;
			return false;
		}
		else if (-simplex[2].Dot(ACperp) > 0)
		{
			simplex.erase(simplex.begin() + 1);
			dir = ACperp;
			return false;
		}
	}
		return true;

		break;
	default:
		break;
	}
}

static constexpr int MAX_EPA_COUNT = 20;

void GetHitInfo(BaseCollider2D* colA, BaseCollider2D* colB, Simplex& simplex, HitInfomation2D* info)
{
	// シンプレックスを反時計回りに補正する
	// 3角形であること前提
	if (CrossProduct2D(simplex[0], simplex[1], simplex[2]) < 0)
	{
		auto save = simplex[1];
		simplex[1] = simplex[2];
		simplex[2] = save;
	}

	// 調査済みフラグ
	std::vector<bool> isChecked{ false };

	// 各エッジにおける最近点
	std::vector<DirectX::SimpleMath::Vector2> nearPoints{};
	isChecked.resize(3);
	nearPoints.resize(3);

	// 最低値
	float minLenSq = FLT_MAX;

	int minIndex = -1;
	DirectX::SimpleMath::Vector2 minP;

	for (int count = 0; true; count++)
	{
		// 設定した回数ループしたら終了
		if (count >= MAX_EPA_COUNT)
		{
			break;
		}
		
		// エッジ数分ループ
		for (int i = 0; i < simplex.size(); i++)
		{
			// 調査済みのエッジならスキップ
			if (isChecked[i]) continue;

			// 調べるエッジの2点を用意
			DirectX::SimpleMath::Vector2 p1 = simplex[i], p2 = simplex[(i + 1) % simplex.size()];

			// エッジ上の原点に最も近い点を取得
			DirectX::SimpleMath::Vector2 nearP = GetNearPointLineToPoint({ 0, 0 }, p1, p2 - p1);

			isChecked[i] = true;
			nearPoints[i] = nearP;
		}

		// 最も短い点を調べる
		for (int i = 0; i < simplex.size(); i++)
		{
			float lenSq = nearPoints[i].LengthSquared();

			if (lenSq < minLenSq)
			{
				minLenSq = lenSq;
				minP = nearPoints[i];
				minIndex = i;
			}
		}

		// サポート点を取得
		minP.Normalize();

		DirectX::SimpleMath::Vector2 newP = SupportAll(colA, colB, minP);

		// 距離を調べループを終わるかを判定
		{
			float edgeLenSq = minP.LengthSquared(), newLenSq = newP.LengthSquared();
			float currentDist = std::sqrt(minLenSq);
			float newDist = newP.Dot(minP); 
			if (newDist - currentDist < 0.001f) { break; }
		}

		simplex.insert(simplex.begin() + minIndex + 1, newP);

		// 調査済みを更新する

		// 新しく追加されたエッジの部分だけfalseに置き換える
		isChecked.resize(simplex.size());
		for (int i = 0; i < simplex.size(); i++)
		{
			isChecked[i] = (i < minIndex || i > minIndex + 1);
		}
		// 新しく追加された部分より後ろをずらす
		auto copy = nearPoints;
		nearPoints.resize(simplex.size());
		for (int i = simplex.size() - 1; i > minIndex + 1; i--)
		{
			nearPoints[i] = copy[i - 1];
		}
	}

	minP.Normalize();

	info->hitLen = std::sqrt(minLenSq);
	info->hitDir = minP;

	info->own = colA->GetOwn();
	info->target = colB->GetOwn();
}

DirectX::SimpleMath::Vector2 GetNearPointLineToPoint(DirectX::SimpleMath::Vector2 point, DirectX::SimpleMath::Vector2 start, DirectX::SimpleMath::Vector2 dir)
{
	Vector2 w = point - start;     // 点からp1への方向ベクトル

	// 線分の長さの2乗
	float l = dir.LengthSquared();
	if (l < 0.0001f) return point; // 点に近い線分の場合

	// 投影比率 t を内積から計算
	float t = w.Dot(dir) / l;

	// 線分上の最近接点
	return start + dir * t;
}
