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
#include "GameLib/Common/Renderer/Renderer.h"

//====================================================//
// 前方宣言
//====================================================//

class Scene;

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
    std::unique_ptr<IRigidBody> m_pReserveRigidBody;
    std::unique_ptr<IRigidBody> m_pRigidBody;

    // その他コンポーネント
    std::vector<std::unique_ptr<BaseComponent>> m_pReserves;
    std::vector<std::unique_ptr<BaseComponent>> m_pComponents;

    // 自身を持つシーンのポインタ
    Scene* m_pScene;

    // アクティブフラグ　
    bool m_isActive;

    // 削除フラグ
    bool m_isDead;

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    GameObject();

    virtual ~GameObject() = default;

    // 各仮想関数
    virtual void Initialize() {};

    virtual void Update(float elapsedTime) {};

    virtual void Render() {};

    virtual void Finalize() {};

    // 基底クラスで必ず行う処理
    void BaseUpdate(float elapsedTime);

    void BaseRender(Renderer& renderer);

    void BaseFinalize();

    //-----------------------------------------------------
    // ゲッター
    //-----------------------------------------------------
    bool IsActive() const { return m_isActive; }

    bool IsDead() const { return m_isDead; }

    //-----------------------------------------------------
    // セッター
    //-----------------------------------------------------
    void SetActive(bool value) { m_isActive = value; }

    void SetScene(Scene* scene) { m_pScene = scene; }

    void Destroy() { m_isDead = true; }

private:
    // コライダーのデバッグ描画をする関数
    void CollidersDebugDraw(Renderer& renderer, int color, bool drawAABB = false);

    // 衝突関連
public:

    // 衝突時に呼び出される関数

    // 3D
    virtual void OnCollisionEnter(BaseCollider* other) {};
    virtual void OnCollisionStay(BaseCollider* other) {};
    virtual void OnCollisionExit(BaseCollider* other) {};
    virtual void OnTriggerEnter(BaseCollider* other) {};
    virtual void OnTriggerStay(BaseCollider* other) {};
    virtual void OnTriggerExit(BaseCollider* other) {};

    // 2D
    virtual void OnCollision2D(BaseCollider2D* other) {};
    virtual void OnTrigger2D(BaseCollider2D* other) {};

public:
    // コンポーネントを追加する関数
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    // コンポーネントを1つ取得する関数
    template<typename T>
    T* GetComponent();

    // コンポーネントをすべて取得する関数
    template<typename T>
    std::vector<T*> GetComponents();

    template<typename T>
    void GetComponents(std::vector<T*>& array);

    // コンポーネントを全て削除する関数
    void RemoveComponents();

    // 予約済みのコンポーネントを登録する関数
    void RegisterComponents();

    // ----- 内部実装 ------- //
private:
    void RegisterComponentToScene(BaseComponent* component);
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
    T* add = nullptr;

    if constexpr (T::TYPE_ID == MAIN_TRANSFORM) return nullptr;

    else if constexpr (T::TYPE_ID == MAIN_RIGIDBODY || T::TYPE_ID == MAIN_RIGIDBODY_2D)
    {
        if (!m_pRigidBody)
        {
            if(T::TYPE_ID == MAIN_RIGIDBODY)
                m_pReserveRigidBody = std::make_unique<RigidBody>(this, std::forward<Args>(args)...);
            else 
                m_pReserveRigidBody = std::make_unique<RigidBody2D>(this, std::forward<Args>(args)...);
        }

        add = static_cast<T*>(m_pReserveRigidBody.get());
    }
    else
    {
        // ユニークポインタを生成
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = comp.get();

        // 配列に追加
        m_pReserves.push_back(std::move(comp));

        add = ptr;
    }

    // 追加したコンポーネントを返す
    return add;
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

    // TransformやRigidBodyなどは専用ポインタから即リターン
    if constexpr (T::TYPE_ID == MAIN_TRANSFORM) return m_pTransform.get();
    if constexpr (T::TYPE_ID == MAIN_RIGIDBODY || T::TYPE_ID == MAIN_RIGIDBODY_2D)
    {
        if (m_pRigidBody) return static_cast<T*>(m_pRigidBody.get());
        return static_cast<T*>(m_pReserveRigidBody.get());
    }

    constexpr uint16_t mask = T::IS_MAIN ? 0xFF00 : 0xFFFF;

    // それ以外は ID 比較でループ
    for (auto& comp : m_pComponents) 
    {
        // コンポーネントが持つ生のIDを、同じマスクで切り落として比較する
        if ((comp->GetID() & mask) == (T::TYPE_ID & mask))
        {
            return static_cast<T*>(comp.get());
        }
    }
    // 予約リストをチェック
    for (auto& comp : m_pReserves)
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

    GetComponents(list);

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
    else if constexpr (T::TYPE_ID == MAIN_RIGIDBODY)
    {
        if (m_pRigidBody) array.push_back(m_pRigidBody.get());
        if (m_pReserveRigidBody) array.push_back(m_pReserveRigidBody.get());
    }

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
        // 予約リストをチェック
        for (auto& reservedComp : m_pReserves)
        {
            // コンポーネントが持つ生のIDを、同じマスクで切り落として比較する
            if ((reservedComp->GetID() & mask) == (T::TYPE_ID & mask))
            {
                array.push_back(static_cast<T*>(reservedComp.get()));
            }
        }
    }

    return;
}
