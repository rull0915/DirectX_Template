//====================================================//
// ファイル名   : ObjectManager2D.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/16
//
// 概要 :
//
// 更新履歴 :
// 2026/04/16 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "GameObject2D.h"
#include <vector>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class ObjectManager2D
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 生成予約中のオブジェクトリスト
    std::vector<GameObject2D*> m_reservations;

    // 管理しているオブジェクトリスト
    std::vector<std::unique_ptr<GameObject2D>> m_objects;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
private:
    ObjectManager2D();

public:
    ~ObjectManager2D();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // インスタンスを返す関数
    static ObjectManager2D& Instance()
    {
        static ObjectManager2D instance;
        return instance;
    }

    // 更新関数
    void Update(float elapsedTime);

    // 描画関数
    void Render();

    void Finalize();

    // オブジェクト生成関数
    template<typename T, typename... Args>
    T* Generate(Args&&... args);

    // オブジェクト削除関数
    template<typename T>
    void Destroy(T* object);

    void AllDestroy();

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------


    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

    // 予約されているオブジェクトを全てリストへ追加する関数
    void AddReservedObject();

    // 死亡オブジェクトを削除する関数
    void RemoveDeadObject();
};

template<typename T, typename ...Args>
inline T* ObjectManager2D::Generate(Args && ...args)
{
    // GameObject2D派生クラスであれば
    if constexpr (std::is_base_of_v<GameObject2D, T>)
    {
        // ポインタを作成
        T* pObj = new T(std::forward<Args>(args)...);

        // 予約リストに追加
        m_reservations.push_back(pObj);

        // 作成したポインタを返す
        return pObj;
    }

    return nullptr;
}

template<typename T>
inline void ObjectManager2D::Destroy(T* object)
{
    // GameObject派生クラスであれば
    if constexpr (std::is_base_of_v<T, GameObject2D>)
    {
        // 削除フラグを立てる
        object->Destoroy();
    }
}
