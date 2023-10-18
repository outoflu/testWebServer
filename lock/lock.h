#ifndef __LOCK_H
#define __LOCK_H

#include <exception>
#include <pthread.h>
#include <semaphore>

/// @brief 信号量
class sem{
public:
    sem(){
        if (!sem_init(&_sem,0,0)){
            throw std::exception();
        }
    }
    sem(int num){
        if (!sem_init(&_sem,0,num)){
            throw std::exception();
        }
    }
    ~sem(){
        sem_destroy(&_sem);
    }
    bool wait(){
        return sem_wait(&_sem)==0;
    }
    bool post(){
        return sem_post(&_sem)==0;
    }

private:
    sem_t _sem;
};

/// @brief 互斥锁
class locker{
private:
    pthread_mutex_t _mutex;
public:
    locker(){
        if (pthread_mutex_init(&_mutex,NULL)!=0){
            throw std::exception();
        }
    }
    ~locker(){
        pthread_mutex_destroy(&_mutex)==0;
    }
    bool lock(){
        return pthread_mutex_lock(&_mutex)==0;
    }
    bool unlock(){
        return pthread_mutex_unlock(&_mutex)==0;
    }
    pthread_mutex_t* get(){
        return &_mutex;
    }
};

class cond
{
public:
    cond(){
        if (pthread_cond_init(&_cond,NULL)!=0){
            throw std::exception();
        }
    }
    ~cond(){
        pthread_cond_destroy(&_cond);
    }
    bool wait(pthread_mutex_t * mutex){
        return pthread_cond_wait(&_cond,mutex)==0;
    }
    bool timewait(pthread_mutex_t* mutex,struct timespec t){
        return pthread_cond_timedwait(&_cond,mutex,&t)==0;
    }
    bool signal(){
        return pthread_cond_signal(&_cond)==0;
    }
    bool broadcast(){
        return pthread_cond_broadcast(&_cond)==0;
    }
private:
    pthread_cond_t _cond;
};

#endif