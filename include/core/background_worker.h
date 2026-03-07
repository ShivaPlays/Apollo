#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <functional>

namespace age::core
{
	class background_worker
	{
	public:
		background_worker();
		~background_worker();

		//No copying
		background_worker(const background_worker& other) = delete;
		background_worker& operator = (const background_worker& other) = delete;

	public:
		void add_job(const std::function<void()>& value);

		size_t get_num_pending_jobs() const;

	protected:

	private:
		void work();

		std::thread m_thread;
		std::condition_variable m_queue_pending;
		mutable std::mutex m_queue_mutex;

		std::queue<std::function<void()>> m_job_queue;

		bool m_exit = false;
	};
}

