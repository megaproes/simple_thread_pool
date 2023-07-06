#pragma once
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <iostream>
class SimpleThreadPool
{
public:
	// Takes the number of threads as an argument, which it uses to create the pool of threads.
	explicit SimpleThreadPool(std::size_t threadCount);

	// Ensures that all threads are stopped and joined
	~SimpleThreadPool();

	SimpleThreadPool(const SimpleThreadPool &) = delete;
	SimpleThreadPool &operator=(const SimpleThreadPool &) = delete;

	// Allows to "post" a task to the thread pool. The task is a function that run on the thread pool.
	template <typename Fnc_T>
	auto Post(Fnc_T task) -> std::future<decltype(task())>;

	// Is run by each thread in the pool. It fetches tasks from the queue and executes them.
	void WorkOn();

	// Stops all the threads and waits for them to finish their current tasks.
	void Destroy();

private:
	// Stores the number of threads in the thread pool.
	size_t m_threadCount;

	// Actual pool of threads.
	std::vector<std::thread> threads;

	// Queue of tasks to be executed.
	std::queue<std::function<void()>> tasks;

	// Allow the threads to wait for a task to be available in the queue.
	std::condition_variable condition;

	// Synchronize access to the tasks queue. It ensures that only one thread can access the queue at a time.
	std::mutex mut;

	// Idicate whether the thread pool should stop. When true, all threads stop executing tasks and the destructor will complete.
	bool stop;
};

SimpleThreadPool::SimpleThreadPool(std::size_t threadCount) : m_threadCount(threadCount), stop(false)
{
	for (size_t i = 0; i < m_threadCount; ++i)
		threads.emplace_back(&SimpleThreadPool::WorkOn, this);
}

SimpleThreadPool::~SimpleThreadPool() { Destroy(); }

template <typename Fnc_T>
auto SimpleThreadPool::Post(Fnc_T task) -> std::future<decltype(task())>
{
	auto packaged_task = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));
	std::future<decltype(task())> future = packaged_task->get_future();
	{
		std::lock_guard<std::mutex> lock(mut);
		tasks.push([packaged_task]()
				 { (*packaged_task)(); });
	}
	condition.notify_one();

	return future;
}

void SimpleThreadPool::WorkOn()
{
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(mut);
			condition.wait(lock, [this]
						{ return this->stop || !this->tasks.empty(); });

			if (this->stop && this->tasks.empty())
				return;

			task = std::move(this->tasks.front());
			this->tasks.pop();
		}

		task();
	}
}

void SimpleThreadPool::Destroy()
{
	{
		std::unique_lock<std::mutex> lock(mut);
		// std::cout << "Assigning stop to true . . .\n";
		stop = true;
	}

	// Wake up all threads.
	condition.notify_all();

	// Wait for all threads to finish.
	for (std::thread &t : threads)
	{
		// std::cout << "Joining thread  with id: " << t.get_id() << "...\n";
		if (t.joinable())
			t.join();
	}
}