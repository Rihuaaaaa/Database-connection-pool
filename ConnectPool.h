#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "MysqlConn.h"
using namespace std;
class ConnectionPool
{
public:
    static ConnectionPool* getConnectPool();

    ConnectionPool(const ConnectionPool& obj) = delete;   //删除默认拷贝构造
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;  //删除默认赋值构造

    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();

private:
    ConnectionPool();   //懒汉式单例，即默认不会实例化对象，什么时候用什么时候new。

    bool parseJsonFile();       //解析json文件的函数，不需要暴露给用户
    
    //两个子线程执行函数
    void produceConnection();
    void recycleConnection();
    
    //初始化 m_minSize个数据库连接函数
    void addConnection();

    //建立连接需要的属性 即用于 MysqlConn类（使用json文件保存一下9个数据）
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;
    //数据库连接池上下限
    int m_minSize;  
    int m_maxSize;  
    //连接超时时长 与最大的空闲时长
    int m_timeout;
    int m_maxIdleTime;


    //连接池，存储的实例就是写好的 MysqlConn*对象
    queue<MysqlConn*> m_connectionQueue;     

    //共享资源就是连接池队列，因此需要锁与条件变量保护连接池
    mutex m_mutexQ;             
    condition_variable m_cond;
};

