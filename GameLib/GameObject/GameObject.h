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
#include <memory>
#include <type_traits>

#include "Components/Components.h"

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
    // トランスフォーム
    std::unique_ptr<Transform> m_pTransform;

    // コンポーネント
    std::vector<std::unique_ptr<BaseComponent>> m_pComponents;

    // アクティブフラグ　
    bool m_isActive;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    GameObject();

    virtual ~GameObject() = default;

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    bool IsActive() const { return m_isActive; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetActive(bool value) { m_isActive = value; }

public:
    // 衝突時に呼び出される関数
    virtual void OnCollision(BaseCollider* other) {};

protected:
    // コンポーネントを追加する関数
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    // コンポーネントを1つ取得する関数
    template<typename T>
    T* GetComponent();

    // コンポーネントをすべて取得する関数
    template<typename T>
    std::vector<T*> GetComponents();
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

    // 配列に追加
    m_pComponents.push_back(std::move(comp));

    return ptr;
}

/// <summary>
/// コンポーネントを1つ取得する関数
/// </summary>
/// <typeparam name="T"></typeparam>
/// <returns></returns>
template<typename T>
inline T* GameObject::GetComponent()
{
    // トランスフォームのみ分岐
    if constexpr (std::is_same_v<T*, Transform*>) 
    {
        return m_pTransform.get();
    }
    else {
        // 全てのコンポーネントを検索
        for (auto& component : m_pComponents)
        {
            T* casted = dynamic_cast<T*>(component.get());

            // 型が一致した場合
            if (casted) return casted;
        }
    }

    return nullptr;
}

/// <summary>
/// 全てのコンポーネントを取得する関数
/// </summary>
/// <typeparam name="T"></typeparam>
/// <returns></returns>
template<typename T>
inline std::vector<T*> GameObject::GetComponents()
{
    std::vector<T*> list;

    // トランスフォームのみ分岐
    if constexpr (std::is_same_v<T*, Transform*>)
    {
        list.push_back(m_pTransform.get());
    }
    else
    {
        // 全てのコンポーネントを検索
        for (auto& component : m_pComponents)
        {
            T* casted = dynamic_cast<T*>(component.get());

            // 型が一致した場合
            if (casted) list.push_back(casted);
        }
    }

    return list;
}
