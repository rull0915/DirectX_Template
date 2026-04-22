//====================================================//
// ファイル名   : StateMachine.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/06
//
// 概要 : ステートマシンクラス
// FSMを使用したステートパターンによる状態の管理を監督する。
//
// 更新履歴 :
// 2026/03/06 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include <map>
#include <string>
#include <functional>

#include "BaseState.h"

//====================================================//
// 前方宣言
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
template<typename Parent>
class StateMachine
{
private:

    //-----------------------------------------------------
    // 定数
    //-----------------------------------------------------


    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

    // 親のポインタ
    Parent* m_pParent;

    // 現在のステート
    BaseState<Parent>* m_nowState;

    // ステート一覧
    std::map<std::string, BaseState<Parent>*> m_states;

    // 変更先のステート名
    std::string m_changeStateName;

public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    StateMachine();
    ~StateMachine();

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------

    // 更新関数
    void Update(float elapsedTime);
    // 所有者を返す関数
    Parent* GetParent() { return m_pParent; }

    // 現在のステート名を返す関数
    std::string GetCurrentStateName();
    // 現在のステートを返す関数
    BaseState<Parent>* GetCurrentState() { return m_nowState; };

    // 特定の名前がステート名と一致しているかチェックする関数
    bool IsCurrentState(const std::string& name);
    // 特定の名前のステートが存在するかチェックする関数
    bool IsStateExist(const std::string& name);

    // 状態の変更を要請する関数
    void RequestChangeState(std::string stateName);

    // 状態の変更をする関数
    void ChangeState();

    // ステートを追加する関数
    void AddState(const std::string& name, BaseState<Parent>* state, bool Change);

    // 指定したステートにルールを追加する関数
    void AddRule(const std::string& stateName, const std::function<bool()> rule, const std::string targetName);

private:

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};

/// <summary>
/// コンストラクタ
/// </summary>
template<typename Parent>
inline StateMachine<Parent>::StateMachine()
{
}

template<typename Parent>
inline StateMachine<Parent>::~StateMachine()
{
}

/// <summary>
/// 更新関数
/// </summary>
/// <param name="elapsedTime">経過時間</param>
template<typename Parent>
inline void StateMachine<Parent>::Update(float elapsedTime)
{
    // ステートの変更要請があれば
    if (m_changeStateName != "")
    {
        ChangeState();  // ステートの変更
    }

    if (m_nowState)
    {
        m_nowState->Update(elapsedTime);    // ステートの更新
        m_nowState->CheckRules();           // ルールのチェック
    }
}

/// <summary>
/// ステートを追加する関数
/// </summary>
/// <param name="name">ステート名</param>
/// <param name="state">ステートのポインタ</param>
/// <param name="Change">追加後そのステートに変更するかどうか</param>
template<typename Parent>
inline void StateMachine<Parent>::AddState(const std::string& name, BaseState<Parent>* state, bool Change)
{
    // 既に存在しているキーなら
    if (m_states.find(name) != m_states.end()) return;  // 追加は行わない

    // ステートの追加
    m_states.emplace(name, state);

    // 所有者の設定
    state->SetMachine(this);
    state->SetParent(m_pParent);
}

template<typename Parent>
inline bool StateMachine<Parent>::IsCurrentState(const std::string& name)
{
    return GetCurrentStateName() == name;
}

template<typename Parent>
inline bool StateMachine<Parent>::IsStateExist(const std::string& name)
{
    return m_states.find(name) != m_states.end();
}

/// <summary>
/// ステート変更要請を受け取る関数
/// </summary>
/// <param name="stateName">変更後のステート名</param>
template<typename Parent>
inline void StateMachine<Parent>::RequestChangeState(std::string stateName)
{
    // 指定した名前のステートがなければ
    if (m_states.find(stateName) == m_states.end()) return;

    // ステートの変更を要請
    m_changeStateName = stateName;
}

/// <summary>
/// ステートの変更を行う関数
/// </summary>
template<typename Parent>
inline void StateMachine<Parent>::ChangeState()
{
    // 現在のステートの終了処理
    if(m_nowState) m_nowState->Finalize();

    // ステートの変更
    m_nowState = m_states[m_changeStateName];

    // 変更要請のリセット
    m_changeStateName = "";

    // 変更後のステートの初期化処理
    m_nowState->Initialize();
}

/// <summary>
/// 
/// </summary>
/// <param name="stateName">ルールを追加するステート名</param>
/// <param name="rule">追加するルール</param>
/// <param name="targetName">移行先のステート名</param>
template<typename Parent>
inline void StateMachine<Parent>::AddRule(const std::string& stateName, const std::function<bool()> rule, const std::string targetName)
{
    // 2つのステートが存在するかをチェック
    bool exist = (m_states.find(stateName) != m_states.end()) && (m_states.find(targetName) != m_states.end());

    // 一方でも存在しなければ追加しない
    if (!exist) return;

    // ルールの追加
    m_states[stateName]->AddRule(TransitionRule(rule, targetName));
}

/// <summary>
/// 現在のステート名を取得する関数
/// </summary>
/// <returns>今のステート名</returns>
template<typename Parent>
inline std::string StateMachine<Parent>::GetCurrentStateName()
{
    for (auto& state : m_states)
    {
        // 今のステート名を返す
        if (state.second == m_nowState) return state.first;
    }

    return "";
}
