#include "SimpleThreadPool.h"
#include <chrono>
#include <random>
// Global mutex for std::cout synchronization
std::mutex coutMutex;

int main()
{
	{
		SimpleThreadPool threadPool(5);

		auto task = []()
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(1, 100);
			int randomNum = dis(gen);

			// Imitating calculations . . .
			std::this_thread::sleep_for(std::chrono::seconds(1));
			return randomNum;
		};

		std::vector<std::future<int>> futures;

		for (int i = 0; i < 100; ++i)
		{
			futures.push_back(threadPool.Post(task));
		}

		for (auto &f : futures)
		{
			std::lock_guard<std::mutex> lock(coutMutex);
			std::cout << "Random number: " << f.get() << '\n';
		}
	} // Forces to call SimpleThreadPool::Destroy()

	std::cout << "Ending up . . .\n";
	return 0;
}