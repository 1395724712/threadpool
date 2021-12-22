#include"threadpool.hpp"

#include<thread>
#include<iostream>

using namespace std;

int func(int a)
{
    cout<<a<<endl;
    return a;
}

int main(){
    // std::thread thr(func,3);

    

    cout<<"running success"<<endl;
    return 0;
}