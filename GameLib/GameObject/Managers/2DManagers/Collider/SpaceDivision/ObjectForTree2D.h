//====================================================//
// ファイル名   : ObjectForTree2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要 : 分木へ登録するオブジェクトです
//
// 更新履歴 :
// 2026/04/16 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "GameLib/GameObject/Components/Collider/2D/BaseCollider2D.h"

//====================================================//
// 前方宣言
//====================================================//
class Cell2D;

//====================================================//
// クラス宣言
//====================================================//
class ObjectForTree2D
{
public:
	Cell2D* m_pCell;			// 登録空間
	BaseCollider2D* m_pObject;				// 判定対象オブジェクト
	ObjectForTree2D* m_pPre;	// 前のObjectForTree2D構造体
	ObjectForTree2D* m_pNext;	// 次のObjectForTree2D構造体

public:
	ObjectForTree2D() 
		: m_pCell{ nullptr }
		, m_pObject{ nullptr }
		, m_pPre{ nullptr }
		, m_pNext{ nullptr }
	{
	}

	virtual ~ObjectForTree2D()
	{
	}

public:
	// 空間から削除する関数
	bool Remove();

	// 空間を登録
	void RegistCell(Cell2D* pCell)
	{
		m_pCell = pCell;
	}

	// 次のオブジェクトへのポインタを取得
	ObjectForTree2D* GetNextObj() const {
		return m_pNext;
	}
};