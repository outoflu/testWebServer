#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "block_queue.h"

class Log
{
public:

    static Log* get_instance(){
        static Log instance;
        return &instance;
    }
    static void *flush_log_thread(void* args){
        return get_instance()->async_write_log();
    }
    bool init(const char* filepath,int close_log,int log_buf_size=8192,
    int split_lines=5000000,int max_queue_size=0);
    void write_log(int level,const char* format,...);
    void flush(void);

private:
    Log();
    virtual ~Log();
    void * async_write_log(){
        std::string single_log;
        while (_log_queue->pop(single_log)){
            _mutex.lock();
            fputs(single_log.c_str(),_fp);
            _mutex.unlock();
        }
    }
    char _dirname[128];
    char _logname[128];
    int _split_lines;
    int _log_buf_size;
    long long _line_count;
    int _today;
    FILE* _fp;
    char* _buf;
    block_queue<std::string>* _log_queue;
    bool _is_async;
    locker _mutex;
    int m_close_log;    
};

#define LOG_DEBUG(format,...) if (0 == m_close_log ) {Log::get_instance()->write_log(0,format,##__VA_ARGS__);Log::get_instance()->flush();}
#define LOG_INFO(format,...) if (0== m_close_log) {Log::get_instance()->write_log(1,format,##__VA_ARGS__);Log::get_instance()->flush();}
#define LOG_WARN(format,...) if (0== m_close_log) {Log::get_instance()->write_log(2,format,##__VA_ARGS__);Log::get_instance()->flush();}
#define LOG_ERROR(format,...) if (0== m_close_log ) { Log::get_instance()->write_log(3,format,##__VA_ARGS__);Log::get_instance()->flush();}

#endif