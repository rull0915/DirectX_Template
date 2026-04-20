//====================================================//
// ファイル名  : ObjectForTree.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2026/04/08
//
// 概要        :  八分木登録用クラスです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "pch.h"
#include "ObjectForTree.h"

#include "Cell.h"

//====================================================//
// 関数の実体宣言
//====================================================//

bool ObjectForTree::Remove()
{
	// 空間に属していなければ終了
	if (!m_pCell)
		return false;

	// 自分を登録している空間に自身を通知
	if (!m_pCell->OnRemove(this))
		return false;

	// 前後のポインタを繋ぎ直す
	if (m_pPre) m_pPre->m_pNext = m_pNext;
	if (m_pNext) m_pNext->m_pPre = m_pPre;

	// 自身のリンクを掃除
	m_pPre = nullptr;
	m_pNext = nullptr;
	m_pCell = nullptr;

	return true;
}