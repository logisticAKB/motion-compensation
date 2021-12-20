#ifndef MOTION_COMPENSATION_THREADPOOL_H
#define MOTION_COMPENSATION_THREADPOOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <functional>

class ThreadPool {

public:
    explicit ThreadPool(unsigned int numThreads, std::condition_variable* notifier);
    ~ThreadPool();

    void add(const std::function<void()>& job);
    bool isProcessing();
    void shutdown();

private:
    void runner();

    std::queue<std::function<void()>> _jobQueue;
    std::vector<std::thread> _pool;
    std::mutex _queueMutex;
    std::mutex _poolMutex;
    std::condition_variable _cond;
    std::condition_variable *_notifier;
    std::atomic<int> _numJobsRunning;
    bool _terminate;
    bool _terminated;

};


#endif //MOTION_COMPENSATION_THREADPOOL_H
