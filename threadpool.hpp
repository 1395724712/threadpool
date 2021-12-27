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
    auto run(F&& func,Args&&...args)->std::future<decltype(func(args...))>
    {
        //1、确认返回future的类型
        using rtnType = decltype(func(args...));

        //2、指向packaged_task对象的智能指针
        //为什么要使用智能指针
        auto taskPtr = std::make_shared<std::packaged_task<rtnType()>>(std::bind(std::forward<F>(func),std::forward<Args>(args)...));

        //3、加入到任务队列的lambda表达式，即void()类型的可调用对象
        auto lambdaTask = [taskPtr](){(*taskPtr)();};
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(maxQueueSize_ != 0 && tasks_.size() == maxQueueSize_)
                notFull_.wait(lock);
            tasks_.emplace(std::move(lambdaTask));
        }

        //4、创建future对象，并返回
        std::future<rtnType> rtnFuture = taskPtr->get_future();
        return rtnFuture;
    }
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