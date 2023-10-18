#ifndef __MYSQL_CON_POOL_H
#define __MYSQL_CON_POOL_H

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>

#include "../lock/lock.h"
#include "../log/log.h"

class connection_pool{

public:
    MYSQL* GetConnection();
    bool ReleaseConnection(MYSQL* conn);
    int GetFreeConn();
    void DestoryPool();
    static connection_pool* GetInstance();
    void init(std::string url,std::string Uesr,std::string Password,std::string DataBase,
    int Port,int MaxConn,int close_log);
private:
    connection_pool();
    ~connection_pool();
    int _maxConn;
    int _curConn;
    int _freeConn;
    locker _lock;
    std::list<MYSQL*> _connList;
    sem _reserve;

public:
    std::string _url;
    std::string _port;
    std::string _user;
    std::string _password;
    std::string _database;
    int m_close_log;
};

class connectionRAII{
public:
    connectionRAII(MYSQL** conn,connection_pool* connPool);
    ~connectionRAII();
private:
    MYSQL* conRAII;
    connection_pool* poolRAII;
};
#endif