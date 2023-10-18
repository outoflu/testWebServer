#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/lock.h"
#include "../CGImysql/mysql_con_pool.h"


template <typename T>
concept hasState=requires(T* pointer){
    pointer->state;
};

template <typename T>
class threadpool
{
public:
    threadpool(int _model,connection_pool* connPool,
    int threadCount=8, int maxRequests=10000);
    ~threadpool();
    bool append_state(T* request,int state);
    bool append(T* request);
private:
    static void* worker(void* arg);
    void run();

private:
    int thread_count;
    int max_requests;
    pthread_t* _threads;
    std::list<T*> _workqueue;
    locker queue_lock;
    sem queue_sem;
    connection_pool* _conPool;
    int actor_model;
};

template <typename T>
threadpool<T>::threadpool(int _model,connection_pool* connPool,
    int threadCount=8, int maxRequests=10000):
    thread_count(threadCount),_conPool(connPool),actor_model(_model),
    max_requests(maxRequests),_threads(nullptr)
{
    if (threadCount<=0||maxRequests<=0)
        throw std::exception();
    _threads=new pthread_t[threadCount];
    if (!_threads)
        throw std::exception();
    for (int i=0;i<thread_count;++i)
    {
        if (pthread_create(_threads+i,NULL,worker,this)!=0)
        {
            delete[] _threads;
            throw std::exception();
        }
        if (pthread_detach(_threads+i)){
            delete[] _threads;
            throw std::exception();
        }
    }
    
}

template <typename T>
threadpool<T>::~threadpool()
{
    delete[] _threads;
}

template <typename T>
bool threadpool<T>::append_state(T* request,int state)
{
    queue_lock.lock();
    if (_workqueue.size()>=max_requests)
    {
        queue_lock.unlock();
        return false;
    }
    request->_state=state;
    _workqueue.push_back(request);
    queue_lock.unlock();
    queue_sem.post();
    return true;
}

template <typename T>
bool threadpool<T>::append(T* request)
{
    queue_lock.lock();
    if (_workqueue.size()>=max_requests){
        queue_lock.unlock();
        return false;
    }
    _workqueue.push_back(request);
    queue_lock.unlock();
    queue_sem.post();
    return true;
}

template <typename T>
void* threadpool<T>::worker(void* arg)
{
    threadpool* pool = (threadpool*) arg;
    pool->run();
    return pool;
}

template <typename T>
void threadpool<T>::run()
{
    while (true)
    {
        queue_sem.wait();
        queue_lock.lock();
        if (_workqueue.empty())
        {
            queue_lock.unlock();
            continue;
        }
        T* request = _workqueue.front();
        _workqueue.pop_front();
        queue_lock.unlock();
        if (1==actor_model)
        {
            if (0==request->_state)
            {
                if (request->read_once())
                {
                    request->improv=1;
                    connectionRAII mysqlcon(&request->mysql,_conPool);
                    request->process();
                }
                else{
                    request->improv=1;
                    request->timer_flag=1;
                }
            }
            else 
            {
                if (request->write())
                {
                    request->improv=1;
                }
                else{
                    request->improv=1;
                    request->timer_flag=1;
                }
            }
        }
        else 
        {
            connectionRAII mysqlcon(&request->mysql,_conPool);
            request->process();
        }
    }
}
#endif