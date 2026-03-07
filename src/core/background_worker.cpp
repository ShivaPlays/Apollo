#include "../../include/core/background_worker.h"

#include <mutex>

namespace age::core
{
	background_worker::background_worker()
		//: m_thread{ &background_worker::work, this }
	{
		// I needed to put the constructor of my thread here to avoid accessing my conditional variable while it is not yet fully constructed
		m_thread = std::thread{ &background_worker::work, this };
	}

	background_worker::~background_worker()
	{
		{
			std::lock_guard lock{ m_queue_mutex };
			m_exit = true;
			m_queue_pending.notify_one();
		}

		m_thread.join();
	}

	void background_worker::add_job(const std::function<void()>& value)
	{
		std::lock_guard lock{ m_queue_mutex };
		m_job_queue.push(value);
		m_queue_pending.notify_one();
	}

	size_t background_worker::get_num_pending_jobs() const
	{
		std::lock_guard lock{ m_queue_mutex };

		return m_job_queue.size();
	}

	void background_worker::work()
	{
		std::function<void()> job;

		while (true)
		{
			{
				std::unique_lock lock{ m_queue_mutex };
				m_queue_pending.wait(lock, [this]() -> bool { return m_exit || !m_job_queue.empty(); });

				if (m_exit) break;

				job = m_job_queue.front();
				m_job_queue.pop();
			}

			job();
		}
	}
}