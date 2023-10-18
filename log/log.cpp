#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include "log.h"
#include <pthread.h>

using namespace std;

Log::Log(){
    _line_count=0;
    _is_async=false;
    _buf=NULL;
    _log_queue=NULL;
}

Log::~Log(){
    if (_buf){
        delete[] _buf;
    }
    if (_log_queue){
        delete _log_queue;
    }
    if (_fp!=NULL){
        fclose(_fp);
    }
}

bool Log::init(const char* filepath,int close_log,int log_buf_size,
int split_lines,int max_queue_size){
    if (max_queue_size>=1){
        _is_async=true;
        _log_queue=new block_queue<string>(max_queue_size);
        pthread_t pt;
        pthread_create(&pt,NULL,flush_log_thread,NULL);
    }

    m_close_log=close_log;
    _log_buf_size=log_buf_size;
    _buf=new char[log_buf_size];
    memset(_buf,'\0',log_buf_size);
    _split_lines=split_lines;

    time_t t=time(NULL);
    struct tm* sys_tm=localtime(&t);
    struct tm _tm=*sys_tm;

    const char* p= strrchr(filepath,'/');
    char log_full_name[256]={0};
    if (p==NULL){
        snprintf(log_full_name,255,
            "%d_%02d_%02d_%s",_tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday,filepath);

    }
    else{
        strcpy(_logname,p+1);
        strncpy(_dirname,filepath,p-filepath+1);
        snprintf(log_full_name,255,
            "%s%d_%02d_%02d_%s",_dirname,_tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday,filepath);

    }

    _today=_tm.tm_mday;
    _fp=fopen(log_full_name,"a");
    if (_fp==NULL) return false;
    return true;
}

void Log::write_log(int level,const char* format,...)
{
    struct timeval now={0,0};
    gettimeofday(&now,NULL);
    time_t t=now.tv_sec;
    struct tm* sys_tm=localtime(&t);
    struct tm _tm=*sys_tm;
    char s[16]={0};
    switch (level)
    {
        case 0:
            strcpy(s,"[debug]:");
            break;
        case 1:
            strcpy(s,"[info]:");
            break;
        case 2:
            strcpy(s,"[warn]:");
            break;
        case 3:
            strcpy(s,"[error]:");
            break;
        default:
            strcpy(s,"[info]:");
    }

    _mutex.lock();
    _line_count++;
    if (_today!=_tm.tm_mday||_line_count%_split_lines==0)
    {
        char new_log[256]={0};
        fflush(_fp);
        fclose(_fp);
        char tail[16]={0};
        snprintf(tail,16,"%d_%02d_%02d",_tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday);
        if (_today!=_tm.tm_mday){
            snprintf(new_log,255,"%s%s%s",_dirname,tail,_logname);
            _today=_tm.tm_mday;
            _line_count=0;
        }
        else{
            snprintf(new_log,255,"%s%s%s.%lld",_dirname,tail,_logname,_line_count/_split_lines);

        }
        _fp=fopen(new_log,"a");

    }
    _mutex.unlock();
    va_list valist;
    va_start(valist,format);
    string log_str;
    _mutex.lock();

    int n=snprintf(_buf,48,"%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
    _tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday,
    _tm.tm_hour,_tm.tm_min,_tm.tm_sec,now.tv_sec,s);

    int m=vsnprintf(_buf+n,_log_buf_size-n-1,format,valist);
    _buf[n+m]='\n';
    _buf[n+m+1]='\0';
    log_str=_buf;

    _mutex.unlock();

    if (_is_async&&!_log_queue->full()){
        _log_queue->push(log_str);
    }else{
        _mutex.lock();
        fputs(log_str.c_str(),_fp);
        _mutex.unlock();
    }

    va_end(valist);
}

void Log::flush(void){
    _mutex.lock();
    fflush(_fp);
    _mutex.unlock();
}