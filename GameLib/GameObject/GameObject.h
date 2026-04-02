//====================================================//
// ファイル名   : GameObject.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/19
//
// 概要 : ゲームオブジェクト用基底クラスです
// 　　   コンポーネントを追加したいオブジェクト等はこれを継承して作成します
//
// 更新履歴 :
// 2026/03/19 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//

#include "../Components/Collider/Shapes/Colliders.h"    // 各コライダー
#include <memory>

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class GameObject
{
    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------
    
private:
    // コライダー
    std::vector<std::unique_ptr<BaseCollider>> m_pColliders;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    GameObject() {};
    virtual ~GameObject();

public:
    virtual void OnCollision(BaseCollider* other) {};

    void AllCollideAccept();

protected:
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    const std::vector<std::unique_ptr<BaseCollider>>& GetCollider() const { return m_pColliders; }
};

/// <summary>
/// コンポーネントを追加する関数
/// </summary>
/// <typeparam name="T">コンポーネントの型</typeparam>
/// <param name="...args">コンポーネントの引数</param>
/// <returns></returns>
template<typename T, typename... Args>
T* GameObject::AddComponent(Args&&... args)
{
    // ユニークポインタを生成
    auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
    T* ptr = comp.get();

    // もしコライダーなら配列に追加
    if constexpr (std::is_base_of_v<BaseCollider, T>) {
        m_pColliders.push_back(std::move(comp));
    }

    return ptr;
}