//====================================================//
// ファイル名   : ObjectManager.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/04/07
//
// 概要 :
//
// 更新履歴 :
// 2026/04/07 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "../GameObject.h"
#include <vector>
#include "GameLib/Common/Renderer/Renderer.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class ObjectManager
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 生成予約中のオブジェクトリスト
    std::vector<GameObject*> m_reservations;

    // 管理しているオブジェクトリスト
    std::vector<std::unique_ptr<GameObject>> m_objects;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
private:
    ObjectManager();

public:
    ~ObjectManager();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // インスタンスを返す関数
    static ObjectManager& Instance()
    {
        static ObjectManager instance;
        return instance;
    }

    // 更新関数
    void Update(float elapsedTime);

    // 描画関数
    void Render(Renderer& renderer);

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
inline T* ObjectManager::Generate(Args && ...args)
{
    // GameObject派生クラスであれば
    if constexpr (std::is_base_of_v<GameObject, T>)
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
inline void ObjectManager::Destroy(T* object)
{
    // GameObject派生クラスであれば
    if constexpr (std::is_base_of_v<GameObject, T>)
    {
        // 削除フラグを立てる
        object->Destory();
    }
}
