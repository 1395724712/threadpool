#include"threadpool.hpp"

#include<assert.h>
#include<utility>
#include<memory>

ThreadPool::ThreadPool(const unsigned int maxQueueSize):
maxQueueSize_(maxQueueSize),
mutex_(),
notFull_(),
notEmpty_(),
run_(false)
{}

ThreadPool::~ThreadPool(){
    stop();

    //执行join
    for(int i = 0;i<threadPool_.size();i++){
        if(threadPool_[i].joinable())
            threadPool_[i].join();
    }

    std::cout<<"ThreadPool running end"<<std::endl;
}

void ThreadPool::stop(){
    run_ = false;

    //通知所有线程
    notFull_.notify_all();
    notEmpty_.notify_all();
}

void ThreadPool::start(const unsigned int threadNum){

    assert(threadNum>0);
    run_ = true;
    for(int i=0;i<threadNum;i++){
        threadPool_.emplace_back(std::bind(&ThreadPool::runInThread,this));
    }
    
}

// template<typename F,typename... Args>
// auto ThreadPool::run(F&& func,Args&&...args)->std::future<decltype(func(args...))>
// {
//     //1、确认返回future的类型
//     using rtnType = decltype(func(args...));

//     //2、指向packaged_task对象的智能指针
//     //为什么要使用智能指针
//     auto taskPtr = std::make_shared<std::packaged_task<rtnType>>(std::bind(std::forward<F>(func),std::forward<Args>(args)...));

//     //3、加入到任务队列的lambda表达式，即void()类型的可调用对象
//     auto lambdaTask = [taskPtr](){(*taskPtr)();};
//     {
//         std::unique_lock<std::mutex> lock(mutex_);
//         while(maxQueueSize_ != 0 && tasks_.size() == maxQueueSize_)
//             notFull_.wait(lock);
//         tasks_.emplace(std::move(lambdaTask));
//     }

//     //4、创建future对象，并返回
//     std::future<rtnType> rtnFuture = taskPtr->get_future();
//     return rtnFuture;
// }

void ThreadPool::runInThread(){

    while(run_){
        std::function<void()> task;

        std::cout<<"current thread Id: "<<std::this_thread::get_id()<<" start to run."<<std::endl;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            //先检查任务队列是否为空
            while(run_ && tasks_.empty())
                notEmpty_.wait(lock);
        
            task = move(tasks_.front());
            tasks_.pop();
        }
        
        std::cout<<"current thread Id: "<<std::this_thread::get_id()<<" running end."<<std::endl;

        task();
    }
}