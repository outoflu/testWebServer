#ifndef __BLOCK_QUEUE_H
#define __BLOCK_QUEUE_H

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "../lock/lock.h"

using namespace std;

template <class T>
class block_queue
{
public:
    block_queue(int max_size = 1000 )
    {
        if (max_size <= 0 )
        {
            exit(-1);
        }
        _max_size=max_size;
        begin=new T[max_size];
        size=0;
        front=-1;
        back=-1;
    }
    void clear(){
        _mutex.lock();
        size=0;
        front=-1;
        back=-1;
        _mutex.unlock();
    }

    ~block_queue(){
        _mutex.lock();
        if (begin!=NULL){
            delete [] begin;
        }
        _mutex.unlock();
    }
    bool full(){
        _mutex.lock();
        if (size>=_max_size){
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }
    bool empty(){
        _mutex.lock();
        if (size==0){
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }

    bool front(T& value){
        _mutex.lock();
        if (0==size){
            _mutex.unlock();
            return false;
        }
        value=array[front];
        _mutex.unlock();
        return true;
    }

    bool back(T& value){
        _mutex.lock();
        if (0==size){
            _mutex.unlock();
            return false;
        }
        value=array[back];
        _mutex.unlock();
        return true;
    }

    int size(){
        int tmp=0;
        _mutex.lock();
        tmp=size;
        _mutex.unlock();
        return tmp;
    }

    int max_size(){
        return _max_size;    
    }

    bool push(const T& item){
        _mutex.lock();
        if (size>=_max_size){
            _cond.broadcast();
            _mutex.unlock();
            return false;
        }
        back=(back+1)%_max_size;
        array[back]=item;
        ++size;
        _cond.broadcast();
        _mutex.unlock();
        return true;
    }

    bool pop(T& item){
        _mutex.lock();
        while (size<=0){
            if (!_cond.wait(_mutex.get())){
                _mutex.unlock();
                return false;
            }
        }

        front=(front+1)%_max_size;
        item=array[front];
        --size;
        _mutex.unlock();
        return true;
    }

    bool pop(T& item,int timeout_ms){
        timespec t ={0,0};
        timeval now={0,0};
        gettimeofday(&now,NULL);
        _mutex.lock();
        if (size<=0){
            t.tv_sec=now.tv_sec+timeout_ms/1000;
            t.tv_nsec = (timeout_ms%1000)*1000;
            if (!_cond.timewait(_mutex.get(),t)){
                _mutex.unlock();
                return false;
            }
        }
        if (size<=0){
            _mutex.unlock();
            return false;
        }

        front = (front+1)%_max_size;
        item = array [ front ];
        --size;
        _mutex.unlock();
        return true;
    }
private:
    locker _mutex;
    cond _cond;
    T* array;
    int size;
    int _max_size;
    int front;
    int back;
};


#endif