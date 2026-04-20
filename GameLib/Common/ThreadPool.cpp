//====================================================//
// ファイル名  : ThreadPool.cpp
// 作成者      : Hoshino Ryunosuke
// 作成日       : 2025/12/04
//
// 概要        :  スレッドプールクラスのcppです
//====================================================//

//====================================================//
// インクルードファイル
//====================================================//
#include "ThreadPool.h"

//====================================================//
// 関数の実体宣言
//====================================================//

ThreadPool::ThreadPool(int threadNum)
	: m_isStop{ false }
{
	// スレッドの作成
	for(int i = 0; i < threadNum; i++)
	{
		m_threads.emplace_back
		(
			// ラムダ式の作成
			[this] {
				// スレッドが実行する内容
				while (true)
				{
					std::function<void()> task;

					// ロック解放用のスコープ
					{
						// ロック
						std::unique_lock<std::mutex> lock(m_mtx);
						
						// ストップフラグがオンか、タスクが空じゃなければ待機をやめる
						m_cv.wait(lock, [this] { return m_isStop || !m_tasks.empty(); });

						// stopかつタスクがければ終了
						if (m_isStop && m_tasks.empty()) return;

						// 先頭のタスクをコピー
						task = m_tasks.front();

						// 先頭のタスクを削除
						m_tasks.pop();
					}

					// タスクの実行
					task();
				}
			}
		);
	}
}

ThreadPool::~ThreadPool()
{
	{
		// ストップフラグを保護
		std::unique_lock<std::mutex> lock(m_mtx);
		m_isStop = true;
	}
	// 全て起こす
	m_cv.notify_all();

	// 全スレッドの待機
	for (auto& thread : m_threads)
		thread.join();
}
