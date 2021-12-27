#include"threadpool.hpp"

#include<thread>
#include<iostream>

using namespace std;

int func(int a)
{
    this_thread::sleep_for(1000ms);
    cout<<a<<endl;
    return a*1000;
}

int main(){
    // std::thread thr(func,3);

    ThreadPool threadPool;
    threadPool.start(5);
    vector<future<int>> resVec;

    for(int i = 0;i<10;i++){
        future<int> tmp = threadPool.run(func,i);
        resVec.push_back(move(tmp));
    }
    
    cout<<"输出结果"<<endl;

    for(int j=0;j<resVec.size();j++){
        cout<<resVec[j].get()<<endl;
    }


    cout<<"running success"<<endl;
    return 0;
}