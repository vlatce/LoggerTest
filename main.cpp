#include <iostream>

#include "aurora/logger.hpp"

using namespace aurora;

static const char* s_Module = "MAIN";

#define M_LOG(l,w) (F_LOG("TestLogger", "MAIN",l,w))

template<typename F, typename... Args>
uint64_t funcTime(F func, Args&&... args) {
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	func(std::forward<Args>(args)...);
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t1).count();
}

int main(int argc, char** argv)
{
	logger<file_policy>::async_logger().start(log_level::kDebug, "TestLogger", "TestLogger.log");

	int thread_count = 10;
	int howmany = 1000000;

	std::cout << funcTime([thread_count, howmany]() {
		std::atomic<int > msg_counter{ 0 };
		std::vector<std::thread> threads;
		for (int t = 0; t < thread_count; ++t)
		{
			threads.push_back(std::thread([&]()
			{
				while (true)
				{
					int counter = ++msg_counter;
					if (counter > howmany) break;
					M_LOG(log_level::kInformation, fmt::format("logger message {0}: This is some text for your pleasure", counter));
				}
			}));
		}

		for (auto &t : threads)
		{
			t.join();
		};
	});

	std::cout.flush();

	logger<file_policy>::async_logger().stop();

	return 0;
}