#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <thread>
#include <fstream>
#include <sstream>
#include <atomic>
#include <mutex>
#include <chrono>
#include <string_view>

#include <boost/filesystem.hpp>

#include "../fmt/format.h"
#include "../queue/concurrentqueue.hpp"

#define F_LOG(n,m,l,w) (logger<file_policy>::async_logger().operator()(n, m, l, w))

namespace aurora
{
enum class log_level : std::uint32_t
{
	kNone = 0,
	kError,
	kInformation,
	kVerbose,
	kDebug,
};

const std::string& log_level_to_str(log_level level)
{
	switch (level)
	{
		case log_level::kNone:
			break;
		case log_level::kError:
		{
			static std::string strLevelName("ERROR");
			return strLevelName;
		}
		break;

		case log_level::kInformation:
		{
			static std::string strLevelName("INFO");
			return strLevelName;
		}
		break;

		case log_level::kVerbose:
		{
			static std::string strLevelName("VERBOSE");
			return strLevelName;
		}
		break;

		case log_level::kDebug:
		{
			static std::string strLevelName("DEBUG");
			return strLevelName;
		}
		break;
	}
	static std::string strLevelNone("NONE");
	return strLevelNone;
}

template<typename T>
std::string time_to_string(std::chrono::time_point<T> time) {
	std::stringstream stream;
	time_t curr_time = T::to_time_t(time);
	struct tm time_info;
	char sRep[100];

#if (WIN32)
	localtime_s(&time_info, &curr_time);
#else
	localtime_r(&curr_time, &time_info);
#endif
	strftime(sRep, sizeof(sRep), "%Y-%m-%d %H:%M:%S", &time_info);

	typename T::duration since_epoch = time.time_since_epoch();
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
	since_epoch -= s;
	std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);

	stream << sRep << "." << milli.count();

	return stream.str();
}

class file_policy
{
public:
	file_policy() = default;
	file_policy(const file_policy&) = delete;

	virtual ~file_policy()
	{
		for (auto log_pair : log_files_)
		{
			log_pair.second->flush();
			log_pair.second->close();
		}
	}
protected:
	void emit(std::string_view name, std::string_view module, log_level level, std::string_view fmt) {
			if (level > log_level_) return;
			if (log_files_.count(name.data()) == 0) return;

			std::string fmtLogEntry = fmt::format("[{0}:{1}:{2}:{3}] {4}", name, time_to_string<std::chrono::system_clock>(std::chrono::system_clock::now()), log_level_to_str(level), module, fmt);

			{
				std::lock_guard<std::mutex> lock(write_mutex_);
				std::shared_ptr<std::ofstream> log_file = log_files_[name.data()];
				log_file->write(fmtLogEntry.c_str(), fmtLogEntry.size());
				*log_file << std::endl;
				log_file->flush();
			}
		}

	bool init(std::string_view name, std::string_view data) { //data is file path
		bool ret_val(false);

		boost::filesystem::path file(data.data());

		if (!(boost::filesystem::exists(file) && boost::filesystem::is_directory(file)))
		{
			std::shared_ptr<std::ofstream> temp_file;

			if (log_files_.count(name.data()) == 0)
			{
#if (WIN32)
				temp_file.reset(new std::ofstream(data.data(), std::ios_base::out | std::ios_base::app | std::ios_base::ate, _SH_DENYNO));
#else
				temp_file.reset(new std::ofstream(data.data(), std::ios_base::out | std::ios_base::app | std::ios_base::ate));
#endif
				log_files_[name.data()] = temp_file;
			}
			else
			{
				temp_file = log_files_[name.data()];
			}

			if (temp_file || temp_file->is_open())
			{
				ret_val = true;
			}
		}

		return ret_val;
	}

protected:
		log_level log_level_;
private:
		std::unordered_map<std::string, std::shared_ptr<std::ofstream>> log_files_;
		std::mutex write_mutex_;
	};


	template <typename output_policy>
	class logger : private output_policy
	{
		using output_policy::emit;
		using output_policy::init;
	public:
		logger() = default;
		logger(const logger&) = delete;
		
		virtual ~logger()
		{
			try
			{
				stop();
			}
			catch (...)
			{

			}
		}
	public:
		void operator()(std::string_view name, std::string_view module, log_level level, const std::string &fmt)
		{
			if (!should_stop_.load())
			{
				//Fast lock-free queue
				queue_.enqueue([this, name, module, level, fmt{ move(fmt) }]() {
					emit(name, module, level, fmt);
				});
			}
		}

public:
		static logger<output_policy>& async_logger()
		{
			static logger<output_policy> global_logger;

			return global_logger;
		}

		bool start(log_level level, std::string_view name, std::string_view policy_data)
		{
			output_policy::log_level_ = level;

			if (!init(name, policy_data)) return false;

			if (worker_threads_.size() == 0)
			{
				worker_threads_.push_back(std::thread([this] {
					std::function<void()> value;

					while (!should_stop_.load())
					{
						if (queue_.try_dequeue(value))
						{
							value();
						}
						else
						{
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
						}
					}

					//empty the queue
					while (queue_.try_dequeue(value))
					{
						value();
					}
				}));
			}

			return true;
		}

		void stop()
		{
			should_stop_.store(true);

			for (auto i = worker_threads_.begin(); i != worker_threads_.end(); ++i)
			{
				if (i->joinable())
					i->join();
			}
		}
	private:
		std::atomic<bool> should_stop_;
		std::vector<std::thread> worker_threads_;
		moodycamel::ConcurrentQueue<std::function<void()>> queue_; //Fast lock-free queue
	};
}//namespace aurora