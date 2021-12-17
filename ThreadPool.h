#ifndef MOTION_COMPENSATION_THREADPOOL_H
#define MOTION_COMPENSATION_THREADPOOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

class ThreadPool {

public:
    explicit ThreadPool(unsigned int numThreads);
    ~ThreadPool();

    void add(const std::function<void()>& job);
    bool queueEmpty();
    void shutdown();

private:
    void runner();

    std::queue<std::function<void()>> _jobQueue;
    std::vector<std::thread> _pool;
    std::mutex _queueMutex;
    std::mutex _poolMutex;
    std::condition_variable _cond;
    std::function<void()> _job;
    bool _terminate;
    bool _terminated;

};


#endif //MOTION_COMPENSATION_THREADPOOL_H
