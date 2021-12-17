#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int numThreads) {
    _terminate = false;
    _terminated = false;

    numThreads = std::min(numThreads, std::thread::hardware_concurrency());
    for (int i = 0; i < numThreads; i++) {
        _pool.emplace_back(std::thread(&ThreadPool::runner, this));
    }
}

void ThreadPool::runner() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _cond.wait(lock, [this]{return !_jobQueue.empty() || _terminate;});

            if (_terminate) return;

            _job = _jobQueue.front();
            _jobQueue.pop();
        }

        _job();
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

bool ThreadPool::queueEmpty() {
    return _jobQueue.empty();
}
