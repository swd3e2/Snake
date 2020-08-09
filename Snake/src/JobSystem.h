#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <queue>

struct Job {
	std::function<void()> func;
};

class JobSystem {
private:
	std::mutex jobLock;
	std::queue<Job> jobs;

	static JobSystem* _instance;

	std::atomic_int hasJobs;
	std::atomic_int processedJobs;

	std::mutex wakeMutex;
	std::condition_variable wakeWorker;
public:
	JobSystem() {
		_instance = this;

		resetJobsCounter();
	}

	static JobSystem* instance() {
		return _instance;
	}

	void addJob(Job&& job) {
		jobLock.lock();
		jobs.push(job);
		jobLock.unlock();

		wakeWorker.notify_one();
		hasJobs.fetch_add(1);
	}

	void executeJob() {
		while (1) {
			// someone blocked, wait unitll it free it
			jobLock.lock();
			if (!jobs.empty()) {
				Job j = jobs.front();
				jobs.pop();

				jobLock.unlock();

				j.func();
				processedJobs.fetch_add(1);
			}
			else
			{
				std::unique_lock<std::mutex> lock(wakeMutex);
				jobLock.unlock();

				wakeWorker.wait(lock);
			}
		}
	}

	void resetJobsCounter()
	{
		hasJobs.store(0);
		processedJobs.store(0);
	}

	bool jobsCompleted()
	{
		return hasJobs.load() == processedJobs.load();
	}
};


class WorkerThread {
public:
	std::thread* th = nullptr;
public:
	WorkerThread() {
		th = new std::thread([]() {
			while (1) {
				JobSystem* system = JobSystem::instance();
				system->executeJob();
			}
			});
		th->detach();
	}
};