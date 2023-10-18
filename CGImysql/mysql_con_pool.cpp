#include "mysql_con_pool.h"

connection_pool::connection_pool(){
    _curConn=0;
    _freeConn=0;
}

connection_pool* connection_pool::GetInstance(){
    static connection_pool _connPool;
    return &_connPool;
}

void connection_pool::init(std::string Url,std::string User,std::string Password,std::string Database,
int Port , int MaxConn,int close_log){
    _url=Url;
    _user=User;
    _password=Password;
    _port=Port;
    _maxConn=MaxConn;
    m_close_log=close_log;

    for (int i=0;i<MaxConn;++i){
        MYSQL* conn =NULL;
        conn=mysql_init(conn);
        if (conn==NULL){
            LOG_ERROR("MYSQL ERROR");
            exit(1);
        }
        conn = mysql_real_connect(conn,Url.c_str(),User.c_str(),Password.c_str(),
                Database.c_str(),Port,NULL,0);
        
        if (conn==NULL){
            LOG_ERROR("MYSQL CONNECT FAILED");
            exit(1);
        }

        _connList.push_back(conn);
        ++_freeConn;
    }
    _reserve=sem(_freeConn);

    _maxConn=_freeConn;
}

MYSQL* connection_pool::GetConnection(){
    MYSQL* conn=NULL;
    if (_connList.empty()){
        return NULL;
    }
    _reserve.wait();
    _lock.lock();
    conn=_connList.front();
    _connList.pop_front();
    --_freeConn;
    ++_curConn;
    _lock.unlock();
    return conn;
}

bool connection_pool::ReleaseConnection(MYSQL* con)
{
    if (NULL==con)
    {
        return false;
    }
    _lock.lock();

    _connList.push_back(con);
    ++_freeConn;
    --_curConn;
    _lock.unlock();
    _reserve.post();
    return true;
}

void connection_pool::DestoryPool()
{
    _lock.lock();
    if (!_connList.empty())
    {
        std::list<MYSQL*>::iterator it;
        for (it=_connList.begin();it!=_connList.end();++it){
            MYSQL* con=* it;
            mysql_close(con);
        }
        _curConn=0;
        _freeConn=0;
        _connList.clear();
    }
    _lock.unlock();
}

int connection_pool::GetFreeConn(){
    return _freeConn;
}

connection_pool::~connection_pool(){
    DestoryPool();
}

connectionRAII::connectionRAII(MYSQL** SQL,connection_pool* connPool)
{
    *SQL=connPool->GetConnection();
    conRAII=*SQL;
    poolRAII=connPool;
}

connectionRAII::~connectionRAII(){
    poolRAII->ReleaseConnection(conRAII);
}