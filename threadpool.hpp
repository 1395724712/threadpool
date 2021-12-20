#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include<vector>
#include<queue>

#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<future>
#include<functional>

#define DEBUG_TEST

class ThreadPool{
public:
    ThreadPool(const unsigned int maxQueueSize = 0);
    ~ThreadPool();

    //创建指定数量的线程数，不得小于1
    void start(const unsigned int threadNum = 1);

    //提交业务函数
    template<typename F,typename... Args>
    auto run(F&& func,Args&&...args) ->std::future<decltype(func(args...))>;

private:
    //停止执行任务
    void stop();

    //每个线程执行函数
    void runInThread();

    //任务队列的最大大小
    const unsigned int maxQueueSize_;

    //用于保护条件变量的互斥锁
    std::mutex mutex_;
    
    //当任务队列已满时阻塞
    std::condition_variable notFull_;
    
    //当任务队列为空时阻塞
    std::condition_variable notEmpty_;

    //线程池
    std::vector<std::thread> threadPool_;

    //任务队列
    std::queue<std::function<void()>> tasks_;

    //是否处于运行状态
    bool run_;
};

#endif