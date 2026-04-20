#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <future>
#include <memory>

const unsigned int THREAD_NUM = std::thread::hardware_concurrency();

// スレッドプールクラス
class ThreadPool
{
	// ----- 変数宣言 ----- //

	// タスクの配列
	std::queue<std::function<void()>> m_tasks;

	// スレッドの配列
	std::vector<std::thread> m_threads;

	// ロック用
	std::mutex m_mtx;
	std::condition_variable m_cv;

	// 停止フラグ
	bool m_isStop;

	// ------ 関数宣言 ------ //
private:
	// コンストラクタ
	// スレッド数を引数
	ThreadPool(int threadNum);

	// デストラクタ
	~ThreadPool();

public:
	// シングルトン

	// インスタンスを返す関数
	static ThreadPool& GetInstance()
	{
		static ThreadPool instance(THREAD_NUM);

		return instance;
	}

	// スレッド数を返す関数
	int GetThreadCount() const { return static_cast<int>(m_threads.size()); }

	// タスク追加関数
	template<class F, class... Args>
	auto AddTask(F&& task, Args&&... args)
		// 可読性向上のために戻り値を後置
		-> std::future<typename std::result_of<F(Args...)>::type>;
};

template<class F, class ...Args>
inline auto ThreadPool::AddTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args ...)>::type>
{
	// 戻り値の型を丸める
	using return_type = typename std::result_of<F(Args ...)>::type;

	// ----- タスクの作成 ----- //
	auto task =
		// 関数を抜けても消えない用にshare
		std::make_shared<
		// futureを返す為にpackaged_task
		std::packaged_task<return_type()>
		>(
			// 引数なしで実行するためにbind
			std::bind(
				// 左辺地でも右辺値でも受け取れるようにforward
				std::forward<F>(f), 
				std::forward<Args>(args)...
			)
		);

	// 返すfutureの作成
	// taskと紐づけ
	std::future<return_type> result = task->get_future();

	// ----- タスクの追加 ----- //

	// unlockの為のスコープ
	{
		// lockの作成
		std::unique_lock<std::mutex> lock(m_mtx);

		// タスクキューへの追加
		// taskをコピーキャプチャし実行
		m_tasks.emplace([task]() { (*task)(); });
	}

	// スレッドを一つ起こす
	m_cv.notify_one();

	// futureを返す
	return result;
}
