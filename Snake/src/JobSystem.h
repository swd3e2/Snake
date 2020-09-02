#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <queue>
#include <vector>

class WorkerThread;

struct Job {
	std::function<void()> func;
};

class JobSystem {
private:
	std::mutex jobLock;
	std::queue<Job> jobs;

	std::atomic_int hasJobs;
	std::atomic_int processedJobs;

	std::mutex wakeMutex;
	std::condition_variable wakeWorker;

	std::vector<WorkerThread*> threads;

	static JobSystem* _instance;
public:
	JobSystem();
	inline static JobSystem* instance() { return _instance; }
	void addJob(Job&& job);
	void executeJob();
	void resetJobsCounter();
	inline bool jobsCompleted() { return hasJobs.load() == processedJobs.load(); }
};


class WorkerThread {
private:
	std::thread* th = nullptr;
public:
	WorkerThread();
};