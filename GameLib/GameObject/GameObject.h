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
    // ---- コンポーネント ---- //

    // 頻繁にアクセスするものや、一つしか持たせたくないものを別個で保持

    // トランスフォーム
    std::unique_ptr<Transform> m_pTransform;

    // リジッドボディ
    std::unique_ptr<RigidBody> m_pRigidBody;

    // その他コンポーネント
    std::vector<std::unique_ptr<BaseComponent>> m_pComponents;

    // アクティブフラグ　
    bool m_isActive;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    GameObject();

    virtual ~GameObject() = default;

protected:

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    bool IsActive() const { return m_isActive; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetActive(bool value) { m_isActive = value; }

    // コライダーのデバッグ描画をする関数
    void CollidersDebugDraw(int color, bool drawAABB = false);

public:
    void BaseOnCollision(BaseCollider* other);

    // 衝突時に呼び出される関数
    virtual void OnCollision(BaseCollider* other) {};

public:
    // コンポーネントを追加する関数
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

public:
    // コンポーネントを1つ取得する関数
    template<typename T>
    T* GetComponent();

    // コンポーネントをすべて取得する関数
    template<typename T>
    std::vector<T*> GetComponents();

    template<typename T>
    void GetComponents(std::vector<T*>& array);
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
    if constexpr (T::TYPE_ID == MAIN_TRANSFORM) return;

    else if constexpr (T::TYPE_ID == MAIN_RIGIDBODY)
    {
        m_pRigidBody = std::make_unique<RigidBody>(this, std::forward<Args>(args)...);

        return m_pRigidBody.get();
    }
    else
    {
        // ユニークポインタを生成
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = comp.get();

        // 配列に追加
        m_pComponents.push_back(std::move(comp));

        return ptr;
    }
}

/// <summary>
/// コンポーネントを1つ取得する関数
/// </summary>
/// <typeparam name="T">取得したい型</typeparam>
/// <returns>コンポーネントのポインタ</returns>
template<typename T>
inline T* GameObject::GetComponent()
{
    // 基底クラスを分岐
    if (T::TYPE_ID == MAIN_BASE) return nullptr;

    // 1. TransformやRigidBodyなどは専用ポインタから即リターン
    if constexpr (T::TYPE_ID == MAIN_TRANSFORM) return m_pTransform.get();
    if constexpr (T::TYPE_ID == MAIN_RIGIDBODY) return m_pRigidBody.get();

    constexpr uint16_t mask = T::IS_MAIN ? 0xFF00 : 0xFFFF;

    // 2. それ以外は ID 比較でループ
    for (auto& comp : m_pComponents) 
    {
        // コンポーネントが持つ生のIDを、同じマスクで切り落として比較する
        if ((comp->GetID() & mask) == (T::TYPE_ID & mask))
        {
            return static_cast<T*>(comp.get());
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

    // 基底クラスを分岐
    if (T::TYPE_ID == MAIN_BASE) return list;

    // 1. TransformやRigidBodyなどは専用ポインタから即追加
    if constexpr (T::TYPE_ID == MAIN_TRANSFORM) list.push_back(m_pTransform.get());
    else if constexpr (T::TYPE_ID == MAIN_RIGIDBODY) list.push_back(m_pRigidBody.get());

    else
    {
        constexpr uint16_t mask = T::IS_MAIN ? 0xFF00 : 0xFFFF;

        // 2. それ以外は ID 比較でループ
        for (auto& comp : m_pComponents)
        {
            // コンポーネントが持つ生のIDを、同じマスクで切り落として比較する
            if ((comp->GetID() & mask) == (T::TYPE_ID & mask))
            {
                list.push_back(static_cast<T*>(comp.get()));
            }
        }
    }

    return list;
}

template<typename T>
inline void GameObject::GetComponents(std::vector<T*>& array)
{
    // 配列の初期化
    array.clear();

    // 基底クラスを分岐
    if (T::TYPE_ID == MAIN_BASE) return;

    // 1. TransformやRigidBodyなどは専用ポインタから即追加
    if constexpr (T::TYPE_ID == MAIN_TRANSFORM) array.push_back(m_pTransform.get());
    else if constexpr (T::TYPE_ID == MAIN_RIGIDBODY) array.push_back(m_pRigidBody.get());

    else
    {
        constexpr uint16_t mask = T::IS_MAIN ? 0xFF00 : 0xFFFF;

        // 2. それ以外は ID 比較でループ
        for (auto& comp : m_pComponents)
        {
            // コンポーネントが持つ生のIDを、同じマスクで切り落として比較する
            if ((comp->GetID() & mask) == (T::TYPE_ID & mask))
            {
                array.push_back(static_cast<T*>(comp.get()));
            }
        }
    }

    return;
}
