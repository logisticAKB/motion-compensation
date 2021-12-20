#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(unsigned int numThreads, std::condition_variable* notifier) {
    _notifier = notifier;

    _numJobsRunning = 0;
    _terminate = false;
    _terminated = false;

    numThreads = std::min(numThreads, std::thread::hardware_concurrency());
    for (int i = 0; i < numThreads; i++) {
        _pool.emplace_back(std::thread(&ThreadPool::runner, this));
    }
}

void ThreadPool::runner() {
    while (true) {
        std::function<void()> _job;

        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _cond.wait(lock, [this]{return !_jobQueue.empty() || _terminate;});

            if (_terminate) return;

            _job = _jobQueue.front();
            _jobQueue.pop();
        }

        _numJobsRunning++;
        _job();
        _numJobsRunning--;
        if (!isProcessing()) _notifier->notify_all();
    }
}

void ThreadPool::add(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _jobQueue.push(job);
    }

    _cond.notify_one();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(_poolMutex);
        _terminate = true;
    }

    _cond.notify_all();

    for (std::thread &th : _pool) {
        th.join();
    }

    _pool.clear();
    _terminated = true;
}

ThreadPool::~ThreadPool() {
    if (!_terminated) shutdown();
}

bool ThreadPool::isProcessing() {
    return (_numJobsRunning > 0) || !_jobQueue.empty();
}
