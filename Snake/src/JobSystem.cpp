#include "JobSystem.h"

JobSystem* JobSystem::_instance = nullptr;

JobSystem::JobSystem()
{
	_instance = this;
	resetJobsCounter();

	for (int i = 0; i < std::thread::hardware_concurrency(); i++) {
		threads.push_back(new WorkerThread());
	}
}

void JobSystem::addJob(Job&& job)
{
	jobLock.lock();
	jobs.push(job);
	jobLock.unlock();

	wakeWorker.notify_one();
	hasJobs.fetch_add(1);
}

void JobSystem::executeJob()
{
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

void JobSystem::resetJobsCounter()
{
	hasJobs.store(0);
	processedJobs.store(0);
}

WorkerThread::WorkerThread()
{
	th = new std::thread([]() {
		JobSystem* system = JobSystem::instance();
		for (;;) {
			system->executeJob();
		}
	});
}
