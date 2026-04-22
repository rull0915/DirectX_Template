//====================================================//
// ファイル名   : BaseState.h
// 作成者       : Hoshino Ryunosuke
// 作成日       : 2026/03/06
//
// 概要 : 各ステートの基底クラス。
//
// 更新履歴 :
// 2026/03/06 新規作成
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "TransitionRule.h"
#include <vector>

//====================================================//
// 前方宣言
//====================================================//

template<typename Parent>
class StateMachine;

//====================================================//
// クラス宣言
//====================================================//
template<typename Parent>
class BaseState
{
    //-----------------------------------------------------
    // Friend指定
    //-----------------------------------------------------
    friend class StateMachine<Parent>;
    
    //-----------------------------------------------------
    // メンバ変数
    //-----------------------------------------------------

private:
    std::vector<TransitionRule> m_myRules;  // 遷移規則の一覧

    StateMachine<Parent>* m_pMachine;   // 所属するマシンのポインタ
    Parent* m_pParent;                  // 所有者のポインタ
        
public:

    //-----------------------------------------------------
    // 生成 / 破棄
    //-----------------------------------------------------
    BaseState();
    virtual ~BaseState() = default;

    //-----------------------------------------------------
    // 公開関数
    //-----------------------------------------------------
    virtual void Initialize() = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual void Draw() const = 0;
    virtual void Finalize() = 0;

    void AddRule(const TransitionRule& rule);

protected:

    Parent* GetParent() { return m_pParent; }
    StateMachine<Parent>* GetMachine() { return m_pMachine; }

private:

    void CheckRules();

    void SetMachine(StateMachine<Parent>* machine) { m_pMachine = machine; }
    void SetParent(Parent* parent) { m_pParent = parent; }

    //-----------------------------------------------------
    // 内部実装
    //-----------------------------------------------------

};

template<typename Parent>
BaseState<Parent>::BaseState()
    : m_pMachine{ nullptr }
    , m_pParent{ nullptr }
{
}

template<typename Parent>
void BaseState<Parent>::CheckRules()
{
    // 全ルールを捜査
    for(auto& rule : m_myRules)
    {
        // ルールが満たされていたら
        if(rule.RuleCondition())
        {
            // ステート遷移のリクエスト
            m_pMachine->RequestChangeState(rule.GetTargetName());

            break;
        }
    }
}

template<typename Parent>
void BaseState<Parent>::AddRule(const TransitionRule& rule)
{
    // ルールを追加する
    m_myRules.push_back(rule);
}
