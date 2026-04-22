//====================================================//
// ファイル名   : ObjectForTree.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/08
//
// 概要 : 分木へ登録するオブジェクトです
//
// 更新履歴 :
// 2026/04/08 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "GameLib/GameObject/Components/Collider/3D/BaseCollider.h"

//====================================================//
// 前方宣言
//====================================================//
class Cell;

//====================================================//
// クラス宣言
//====================================================//
class ObjectForTree
{
public:
	Cell* m_pCell;			// 登録空間
	BaseCollider* m_pObject;				// 判定対象オブジェクト
	ObjectForTree* m_pPre;	// 前のObjectForTree構造体
	ObjectForTree* m_pNext;	// 次のObjectForTree構造体

public:
	ObjectForTree() 
		: m_pCell{ nullptr }
		, m_pObject{ nullptr }
		, m_pPre{ nullptr }
		, m_pNext{ nullptr }
	{
	}

	virtual ~ObjectForTree()
	{
	}

public:
	// 空間から削除する関数
	bool Remove();

	// 空間を登録
	void RegistCell(Cell* pCell)
	{
		m_pCell = pCell;
	}

	// 次のオブジェクトへのポインタを取得
	ObjectForTree* GetNextObj() const {
		return m_pNext;
	}
};